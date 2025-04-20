import { html, reactive } from "https://esm.sh/@arrow-js/core"
import { Link } from "./router.js"
import { upperFirst, hideSidebar } from "../js/utils.js"

const MenuItems = {
  home: [
    { name: "Home", link: "/", icon: "bi-house-fill" },
  ],
  navigation: [
    { name: "Manage Keys", link: "/navigation_keys", icon: "bi-key-fill" },
    { name: "Set Destination", link: "/navigation", icon: "bi-globe-americas" },
  ],
  recordings: [
    { name: "Dashcam Routes", link: "/routes", icon: "bi-camera-reels" },
    { name: "Saved Routes", link: "/saved_routes", icon: "bi-box2-heart" },
    { name: "Screen Recordings", link: "/screen_recordings", icon: "bi-record-circle" },
  ],
  tools: [
    { name: "Capture Tmux Log", link: "", icon: "bi-terminal" },
    { name: "Download Speed Limits", link: "", icon: "bi-download" },
    { name: "Error logs", link: "/error_logs", icon: "bi-exclamation-triangle" },
    { name: "Lock/Unlock Doors", link: "", icon: "bi-door-closed" },
    { name: "Toggles", link: "", icon: "bi-toggle-on" },
  ],
}

export function Sidebar() {
  const currentPath = window.location.pathname
  const activeItem = Object.values(MenuItems).flat().find(item => item.link === currentPath)
  const state = reactive({ activeRoute: activeItem?.name ?? "" })

  function navigate(link) {
    state.activeRoute = link.name
    window.scrollTo(0, 0)
    hideSidebar()

    document.querySelectorAll('.sidebar li').forEach(el => {
      el.classList.remove('active')
    })

    const linkElement = document.querySelector(`.sidebar li a[href="${link.link}"]`)
    if (linkElement) {
      linkElement.parentElement.classList.add('active')
    }
  }

  return html`
    <div id="sidebarUnderlay" class="hidden"></div>
    <div id="sidebar" class="sidebar">
      <div>
        <div class="title">
          <img class="logo" src="/assets/images/main_logo.png" alt="FrogPilot logo" />
          <div class="title_text sidebar_header">
            <p>The Pond</p>
            <a href="https://github.com/Aidenir">by&nbsp;Aidenir</a>
          </div>
        </div>
        <hr />
        ${Object.entries(MenuItems).map(([section, links]) => html`
          <div class="sidebar_widget">
            <ul class="menu_section">
              <li>
                <span class="section-title">${upperFirst(section)}</span>
                <ul id="${section}">
                  ${links.map(link => {
                    const isActive = state.activeRoute === link.name
                    const classList = [isActive && "active"].filter(Boolean).join(" ")

                    const content = html`
                      <div class="menu-item-link">
                        <i class="bi ${link.icon}"></i>
                        <span>${upperFirst(link.name)}</span>
                      </div>
                    `

                    return html`
                      <li class="${classList}">
                        ${Link(link.link, content, () => navigate(link))}
                      </li>
                    `
                  })}
                </ul>
              </li>
            </ul>
          </div>
        `)}
      </div>
    </div>`
}

function setupMenuButton() {
  const button = document.getElementById("menu_button")
  const sidebar = document.getElementById("sidebar")
  const underlay = document.getElementById("sidebarUnderlay")

  button.addEventListener("click", () => {
    sidebar.classList.toggle("visible")
    underlay.classList.toggle("hidden")
  })

  underlay.addEventListener("click", hideSidebar)
}

document.addEventListener("DOMContentLoaded", setupMenuButton, false)
