import { html, reactive } from "https://esm.sh/@arrow-js/core"
import { Navigate } from "/assets/components/router.js"

const endpointOptionsCache = {}
const endpointOptionsInflight = {}

// Plain variables — scheduling/routing flags that must NOT be reactive
let syncScheduled = false
let lastParams = null

// Module-level state (persists across route changes)
const state = reactive({
  layout: [],
  allKeys: [],
  paramMetaByKey: {},
  values: {},
  loadingLayout: true,
  loadingValues: true,
  filter: "",
  expanded: {},
  fetched: false,
  activeSectionSlug: "",
})

function slugifySectionName(name) {
  return String(name || "")
    .toLowerCase()
    .replace(/[^a-z0-9]+/g, "-")
    .replace(/^-+|-+$/g, "")
}

function getSectionsWithSlug() {
  return state.layout.map(section => ({
    ...section,
    slug: slugifySectionName(section.name),
  }))
}

function toSelectValue(value) {
  return value === null || value === undefined ? "" : String(value)
}

function resolveEndpointTemplate(template) {
  if (!template) return ""
  return String(template).replace(/\{([A-Za-z0-9_]+)\}/g, (_, key) => {
    return encodeURIComponent(toSelectValue(state.values[key]))
  })
}

function scheduleSyncInputs() {
  if (syncScheduled) return
  syncScheduled = true
  requestAnimationFrame(() => {
    syncScheduled = false
    syncInputs()
  })
}

function applySelectOptions(el, options) {
  el.innerHTML = ""
  for (const opt of options || []) {
    const o = document.createElement("option")
    o.value = String(opt.value)
    o.textContent = opt.label
    el.appendChild(o)
  }
}

async function hydrateEndpointOptions(el, key, endpoint) {
  if (endpointOptionsCache[endpoint]) {
    applySelectOptions(el, endpointOptionsCache[endpoint])
    el.dataset.hydrated = "1"
    el.value = toSelectValue(state.values[key])
    return
  }

  if (!endpointOptionsInflight[endpoint]) {
    endpointOptionsInflight[endpoint] = fetch(endpoint)
      .then(r => r.json())
      .then(options => {
        endpointOptionsCache[endpoint] = options
        return options
      })
      .catch(() => null)
      .finally(() => {
        delete endpointOptionsInflight[endpoint]
      })
  }

  const options = await endpointOptionsInflight[endpoint]
  if (!options || !el.isConnected) return

  applySelectOptions(el, options)
  el.dataset.hydrated = "1"
  el.value = toSelectValue(state.values[key])
}

function syncInputs() {
  // Sync checkboxes — set DOM property directly (attribute alone is unreliable)
  for (const el of document.querySelectorAll("input[type='checkbox'].ds-toggle[id^='ds-']")) {
    el.checked = !!state.values[el.id.slice(3)]
  }

  // Sync selects — hydrate options + set value
  for (const el of document.querySelectorAll("select.ds-select[id^='ds-']")) {
    const key = el.id.slice(3)
    const endpointTemplate = el.getAttribute("data-endpoint")
    const endpoint = resolveEndpointTemplate(endpointTemplate)
    const inlineOptions = state.paramMetaByKey[key]?.options

    if (endpoint) {
      if (!el.dataset.hydrated || el.dataset.endpoint !== endpoint) {
        el.dataset.endpoint = endpoint
        hydrateEndpointOptions(el, key, endpoint)
      } else {
        el.value = toSelectValue(state.values[key])
      }
      continue
    }

    if (Array.isArray(inlineOptions) && inlineOptions.length > 0) {
      if (!el.dataset.hydrated) {
        applySelectOptions(el, inlineOptions)
        el.dataset.hydrated = "1"
      }
      el.value = toSelectValue(state.values[key])
    }
  }
}

async function fetchLayoutAndParams() {
  state.loadingLayout = true
  state.loadingValues = true

  try {
    const layoutRes = await fetch("/assets/components/tools/device_settings_layout.json")
    const rawLayoutData = await layoutRes.json()

    const layoutData = rawLayoutData
      .map(section => ({
        ...section,
        params: (section.params || []).filter(param => param.key !== "Model"),
      }))
      .filter(section => section.params.length > 0)

    state.layout = layoutData

    const keys = []
    const paramMetaByKey = {}
    for (const section of layoutData) {
      for (const p of section.params) {
        keys.push(p.key)
        paramMetaByKey[p.key] = p
      }
    }

    state.allKeys = keys
    state.paramMetaByKey = paramMetaByKey
  } catch (e) {
    console.error("Failed to fetch UI layout:", e)
  }
  state.loadingLayout = false

  // Pull params once at page load; local state handles subsequent edits.
  try {
    const res = await fetch("/api/params/all")
    const data = await res.json()
    state.values = data
  } catch (e) {
    console.error("Failed to fetch param values:", e)
  }
  state.loadingValues = false

  // Resolve slug now that layout is available (uses stored route params)
  resolveActiveSectionSlug(lastParams)
  scheduleSyncInputs()
}

function formatSliderValue(val, stepStr, precisionInt, key) {
  if (val === null || val === undefined) return "--"
  const v = parseFloat(val)
  if (Number.isNaN(v)) return val

  const volumeKeys = [
    "DisengageVolume", "EngageVolume", "PromptVolume",
    "PromptDistractedVolume", "RefuseVolume",
    "WarningImmediateVolume", "WarningSoftVolume",
  ]
  if (key && volumeKeys.includes(key)) {
    if (v === 0) return "Muted"
    if (v === 101) return "Auto"
    return `${v}%`
  }

  if (precisionInt !== undefined && precisionInt !== null) {
    return Number(v.toFixed(precisionInt)).toString()
  }

  if (!stepStr || !stepStr.includes(".")) return Math.round(v).toString()
  const dec = stepStr.split(".")[1].length
  return Number(v.toFixed(dec)).toString()
}

function numericBounds(param) {
  const defaultBounds = {
    min: param.min !== undefined ? param.min : (param.data_type === "float" ? 0.0 : 0),
    max: param.max !== undefined ? param.max : (param.data_type === "float" ? 100.0 : 100),
    step: param.step !== undefined ? param.step : (param.data_type === "float" ? 0.01 : 1),
  }

  const toFinite = (value) => {
    const n = Number(value)
    return Number.isFinite(n) ? n : null
  }

  if (param.key === "ScreenBrightness") {
    return { min: 1, max: 101, step: 1 }
  }
  if (param.key === "ScreenBrightnessOnroad") {
    return { min: 0, max: 101, step: 1 }
  }

  if (param.key === "SteerKP") {
    const base = toFinite(state.values.SteerKPStock) || toFinite(state.values.SteerKP) || 0.6
    return { min: +(base * 0.5).toFixed(2), max: +(base * 1.5).toFixed(2), step: 0.01 }
  }
  if (param.key === "SteerLatAccel") {
    const base = toFinite(state.values.SteerLatAccelStock) || toFinite(state.values.SteerLatAccel) || 2.0
    return { min: +(base * 0.5).toFixed(2), max: +(base * 1.25).toFixed(2), step: 0.01 }
  }
  if (param.key === "SteerRatio") {
    const base = toFinite(state.values.SteerRatioStock) || toFinite(state.values.SteerRatio) || 15.0
    return { min: +(base * 0.25).toFixed(2), max: +(base * 1.5).toFixed(2), step: 0.01 }
  }

  return defaultBounds
}

function coerceValueByType(rawValue, dataType) {
  if (dataType === "int") {
    const n = Number.parseInt(rawValue, 10)
    return Number.isFinite(n) ? n : rawValue
  }
  if (dataType === "float") {
    const n = Number.parseFloat(rawValue)
    return Number.isFinite(n) ? n : rawValue
  }
  return rawValue
}

async function updateParam(key, elType) {
  const current = state.values[key]
  const el = document.getElementById(`ds-${key}`)
  if (!el) return

  const param = state.paramMetaByKey[key] || {}

  let formattedVal
  if (elType === "checkbox") {
    formattedVal = !!el.checked
  } else if (elType === "dropdown") {
    formattedVal = coerceValueByType(el.value, param.data_type)
  } else {
    formattedVal = coerceValueByType(el.value, param.data_type)
  }

  try {
    const res = await fetch("/api/params", {
      method: "PUT",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ key, value: formattedVal }),
    })
    const data = await res.json()

    if (res.ok) {
      const updated = (data.updated && typeof data.updated === "object") ? data.updated : {}
      state.values = { ...state.values, [key]: formattedVal, ...updated }
      showSnackbar(data.message || `${key} updated`)
      scheduleSyncInputs()
    } else {
      revertInput(key, current, elType)
      showSnackbar(data.error || "Failed to update parameter")
    }
  } catch (e) {
    revertInput(key, current, elType)
    showSnackbar("Network error — is the device reachable?")
  }
}

function revertInput(key, current, elType) {
  const el = document.getElementById(`ds-${key}`)
  if (!el) return

  if (elType === "checkbox") {
    el.checked = !!current
    return
  }

  if (elType === "dropdown") {
    el.value = toSelectValue(current)
    return
  }

  el.value = current
  const displayEl = document.getElementById(`ds-display-${key}`)
  if (displayEl) {
    const precision = el.getAttribute("data-precision")
    const pInt = precision ? parseInt(precision, 10) : null
    displayEl.textContent = formatSliderValue(current, el.getAttribute("step"), pInt, key)
  }
}

function handleSliderInput(e, key) {
  const displayEl = document.getElementById(`ds-display-${key}`)
  if (!displayEl) return

  const el = e.target
  const precision = el.getAttribute("data-precision")
  const pInt = precision ? parseInt(precision, 10) : null
  displayEl.textContent = formatSliderValue(el.value, el.getAttribute("step"), pInt, key)
}

function toggleManage(key) {
  state.expanded = { ...state.expanded, [key]: !state.expanded[key] }
  scheduleSyncInputs()
}

function matchesFilter(p) {
  if (!state.filter) return true
  const q = state.filter.toLowerCase()
  return p.label.toLowerCase().includes(q) || p.key.toLowerCase().includes(q)
}

function renderSettingRow(p) {
  if (p.parent_key && !state.filter) {
    if (!state.values[p.parent_key]) return ""
    if (!state.expanded[p.parent_key]) return ""
  }

  const isNumeric = p.ui_type === "numeric"
  const isChild = p.parent_key ? "ds-child-modifier" : ""

  return html`
    <div class="ds-row ${isNumeric ? "ds-row-numeric" : ""} ${isChild}">
      <div class="ds-row-info">
        <div class="ds-row-text">
          <span class="ds-row-label">${p.label}</span>
          ${p.description ? html`<div class="ds-row-desc">${p.description}</div>` : ""}

          ${() => p.is_parent_toggle && state.values[p.key] ? html`
            <div class="ds-manage-btn" @click="${() => toggleManage(p.key)}">
              ${state.expanded[p.key] ? "Close" : "Manage"}
              <i class="bi bi-chevron-${state.expanded[p.key] ? "up" : "down"}"></i>
            </div>
          ` : ""}
        </div>
        ${isNumeric ? html`<span class="ds-row-value" id="ds-display-${p.key}">${state.values[p.key] !== undefined ? formatSliderValue(state.values[p.key], p.step !== undefined ? String(p.step) : undefined, p.precision, p.key) : ".."}</span>` : ""}
      </div>

      ${isNumeric ? html`
        <div class="ds-slider-container">
          ${(() => {
        const bounds = numericBounds(p)
        return html`
              <input
                type="range"
                class="ds-slider"
                id="ds-${p.key}"
                min="${bounds.min}"
                max="${bounds.max}"
                step="${bounds.step}"
                data-precision="${p.precision !== undefined ? p.precision : ""}"
                value="${state.values[p.key] !== undefined ? state.values[p.key] : ""}"
                @input="${(e) => handleSliderInput(e, p.key)}"
                @change="${() => updateParam(p.key, "numeric")}" />
            `
      })()}
        </div>
      ` : p.ui_type === "dropdown" ? html`
        <select
          class="ds-select"
          id="ds-${p.key}"
          data-endpoint="${p.options_endpoint || ""}"
          @change="${() => updateParam(p.key, "dropdown")}">
          <option value="">Loading...</option>
        </select>
      ` : html`
        <input
          type="checkbox"
          class="ds-toggle"
          id="ds-${p.key}"
          @change="${() => updateParam(p.key, "checkbox")}" />
      `}
    </div>
  `
}

// Resolve the active section slug imperatively — NEVER inside a reactive expression
function resolveActiveSectionSlug(params) {
  if (state.layout.length === 0) return

  const sections = getSectionsWithSlug()
  const validSlugs = new Set(sections.map(s => s.slug))
  const requestedSlug = String(params?.section || "").toLowerCase()
  const fallbackSlug = sections[0].slug
  const nextSlug = validSlugs.has(requestedSlug)
    ? requestedSlug
    : (validSlugs.has(state.activeSectionSlug) ? state.activeSectionSlug : fallbackSlug)

  if (state.activeSectionSlug !== nextSlug) {
    state.activeSectionSlug = nextSlug
  }
}

export function DeviceSettings({ params }) {
  lastParams = params

  if (!state.fetched) {
    state.fetched = true
    fetchLayoutAndParams()
  }

  // Resolve slug imperatively (safe: runs in function body, not reactive context)
  resolveActiveSectionSlug(params)

  return html`
    <div class="ds-wrapper">
      <h2>Device Settings</h2>

      <input
        class="ds-search"
        type="text"
        placeholder="Search settings..."
        @input="${(e) => {
      state.filter = e.target.value
      scheduleSyncInputs()
    }}" />

      ${() => {
      if (state.loadingLayout || state.loadingValues) {
        return html`<div class="ds-loading">Loading configuration...</div>`
      }

      const sections = getSectionsWithSlug()
      if (sections.length === 0) {
        return html`<div class="ds-empty">No settings available.</div>`
      }

      // Sync DOM inputs after ArrowJS renders (safe: syncScheduled is non-reactive)
      scheduleSyncInputs()

      // Search active → show matching results from ALL sections
      if (state.filter) {
        const MAX_PER_SECTION = 25
        const searchResults = sections
          .map(s => ({ ...s, matches: s.params.filter(p => matchesFilter(p)) }))
          .filter(s => s.matches.length > 0)

        const totalMatches = searchResults.reduce((n, s) => n + s.matches.length, 0)

        return html`
          <div class="ds-status-bar">
            <span>${totalMatches} result${totalMatches !== 1 ? "s" : ""} across ${searchResults.length} section${searchResults.length !== 1 ? "s" : ""}</span>
            <span>${state.allKeys.length} total mapped</span>
          </div>

          ${searchResults.map(section => html`
            <div class="ds-section">
              <div class="ds-section-header ds-static-header">
                <i class="bi ${section.icon}"></i>
                <span class="ds-section-title">${section.name} (${section.matches.length})</span>
              </div>
              <div class="ds-section-body">
                ${section.matches.slice(0, MAX_PER_SECTION).map(p => renderSettingRow(p))}
                ${section.matches.length > MAX_PER_SECTION ? html`<div class="ds-row"><span class="ds-row-label" style="opacity:0.5">+${section.matches.length - MAX_PER_SECTION} more — refine your search</span></div>` : ""}
              </div>
            </div>
          `)}

          ${totalMatches === 0 ? html`<div class="ds-empty">No settings match your search.</div>` : ""}
        `
      }

      // No search → normal tab-based single-section view
      const activeSection = sections.find(s => s.slug === state.activeSectionSlug) || sections[0]
      const visibleParams = activeSection.params.filter(p => matchesFilter(p))

      return html`
          <div class="ds-tabs">
            ${sections.map(section => html`
              <button
                class="ds-tab ${section.slug === state.activeSectionSlug ? "active" : ""}"
                @click="${() => {
          if (section.slug !== state.activeSectionSlug) {
            Navigate("/device_settings/" + section.slug)
          }
        }}">
                <i class="bi ${section.icon}"></i>
                <span>${section.name}</span>
              </button>
            `)}
          </div>

          <div class="ds-status-bar">
            <span>${activeSection.params.length} settings in ${activeSection.name}</span>
            <span>${state.allKeys.length} total mapped</span>
          </div>

          <div class="ds-section">
            <div class="ds-section-header ds-static-header">
              <i class="bi ${activeSection.icon}"></i>
              <span class="ds-section-title">${activeSection.name} (${visibleParams.length})</span>
            </div>
            <div class="ds-section-body">
              ${visibleParams.map(p => renderSettingRow(p))}
            </div>
          </div>

          ${visibleParams.length === 0 ? html`<div class="ds-empty">No settings match your search.</div>` : ""}
        `
    }}
    </div>
  `
}
