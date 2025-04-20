import { html } from "https://esm.sh/@arrow-js/core"
import { formatSecondsToHuman, parseErrorLogToDate } from "../js/utils.js"

export function ErrorLogs() {
  const state = {
    files: [],
    selectedLog: undefined,
    loading: true,
  }

  async function getErrorLogs() {
    const response = await fetch("/api/error_logs", {
      headers: { Accept: "application/json" },
    })
    const data = await response.json()

    state.files = data.map((file) => {
      const date = parseErrorLogToDate(file)
      return {
        filename: file,
        date: date.toLocaleString(),
        timeSince: (Date.now() - date.getTime()) / 1000,
      }
    })

    state.loading = false
  }

  getErrorLogs()

  return html`
    <h1>Error Logs</h1>
    <div id="errorLogs">
      <div id="fileList">
        ${() => {
          if (state.loading) {
            return html`<div class="fileEntry"><p>Loading...</p></div>`
          }
          return state.files.map(
            (file) => html`
              <div
                class="fileEntry"
                @click="${() => (state.selectedLog = file.filename)}"
              >
                <p>${file.date}</p>
                <p>${formatSecondsToHuman(file.timeSince, "days")} ago</p>
              </div>
            `
          )
        }}
      </div>
      ${() =>
        state.selectedLog
          ? Logviewer(state.selectedLog, () => (state.selectedLog = undefined))
          : ""}
    </div>
  `
}

function Logviewer(filename, closeFn) {
  const logState = {
    loading: true,
    content: undefined,
  }

  async function getLogfile() {
    const response = await fetch(`/api/error_logs/${filename}`, {
      headers: { Accept: "application/json" },
    })
    logState.content = await response.text()
    logState.loading = false
  }

  getLogfile()

  return html`
    <div id="fileViewer">
      <div>
        <p>${filename}</p>
        <button @click="${closeFn}"><i class="bi bi-x-lg"></i></button>
      </div>
      <pre>${() => (logState.loading ? "Loading..." : logState.content)}</pre>
    </div>
  `
}
