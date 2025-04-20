/**
 * Format a number of seconds to a human readable string
 * @param {number} seconds
 * @param {"days"|"hours"|"minutes"} precision
 * @returns {string}
 */
export function formatSecondsToHuman(seconds, precision = "minutes") {
  const units = [
    { label: "days", value: Math.floor(seconds / 86400) },
    { label: "hours", value: Math.floor((seconds % 86400) / 3600) },
    { label: "minutes", value: Math.floor((seconds % 3600) / 60) }
  ]

  const slice = precision === "days" ? 1 : precision === "hours" ? 2 : 3
  return units
    .filter(u => u.value > 0)
    .slice(0, slice)
    .map(u => `${u.value} ${u.label}`)
    .join(", ")
}

/**
 * Parse a filename into a Date
 * Expected format: YYYY-MM-DD--HH-MM-SS
 * @param {string} filename
 * @returns {Date}
 */
export function parseErrorLogToDate(filename) {
  const [date, time] = filename.split("--")
  const [year, month, day] = date.split("-").map(Number)
  const [hour, minute, second] = time.split("-").map(Number)

  return new Date(year, month - 1, day, hour, minute, second)
}

/**
 * Capitalize the first character of a string
 * @param {string} str
 * @returns {string}
 */
export function upperFirst(str) {
  return str ? str[0].toUpperCase() + str.slice(1) : ""
}

/**
 * Show the sidebar (mobile)
 */
export function showSidebar() {
  const html = document.documentElement
  document.getElementById("sidebar")?.classList.add("visible")
  document.getElementById("sidebarUnderlay")?.classList.remove("hidden")
  html.classList.add("no_scroll")
}

/**
 * Hide the sidebar (mobile)
 */
export function hideSidebar() {
  const html = document.documentElement
  document.getElementById("sidebar")?.classList.remove("visible")
  document.getElementById("sidebarUnderlay")?.classList.add("hidden")
  html.classList.remove("no_scroll")
}
