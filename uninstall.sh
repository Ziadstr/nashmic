#!/usr/bin/env bash
# ============================================================================
#  NashmiC Uninstaller - بدك تفك ارتباط؟
#  Cleanly removes NashmiC from your system
#
#  Usage:
#    ./uninstall.sh
#    bash uninstall.sh
# ============================================================================

set -euo pipefail

# ── Configuration ──────────────────────────────────────────────────────────

NASHMIC_HOME="${HOME}/.nashmic"

# ── Colors ─────────────────────────────────────────────────────────────────

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
BOLD='\033[1m'
DIM='\033[2m'
RESET='\033[0m'

if [ ! -t 1 ]; then
    RED='' GREEN='' YELLOW='' BLUE='' CYAN='' BOLD='' DIM='' RESET=''
fi

# ── Output Helpers ─────────────────────────────────────────────────────────

info()    { printf "${BLUE}[*]${RESET} %s\n" "$1"; }
success() { printf "${GREEN}[✓]${RESET} %s\n" "$1"; }
warn()    { printf "${YELLOW}[!]${RESET} %s\n" "$1"; }
error()   { printf "${RED}[✗]${RESET} %s\n" "$1" >&2; }
step()    { printf "${CYAN}[→]${RESET} ${BOLD}%s${RESET}\n" "$1"; }

# ── Shell Profile Cleanup ─────────────────────────────────────────────────

detect_shell_profiles() {
    local profiles=()

    if [ -f "${HOME}/.bashrc" ]; then
        profiles+=("${HOME}/.bashrc")
    fi

    if [ -f "${HOME}/.zshrc" ]; then
        profiles+=("${HOME}/.zshrc")
    fi

    if [ -f "${HOME}/.profile" ] && [ ${#profiles[@]} -eq 0 ]; then
        profiles+=("${HOME}/.profile")
    fi

    echo "${profiles[@]}"
}

remove_shell_config() {
    local profiles
    profiles=$(detect_shell_profiles)

    for profile in ${profiles}; do
        if [ -f "${profile}" ] && grep -qF "# NashmiC — added by installer" "${profile}" 2>/dev/null; then
            sed -i '/# NashmiC — added by installer/d' "${profile}"
            sed -i '/export NASHMIC_ROOT="\$HOME\/.nashmic"/d' "${profile}"
            sed -i '/export PATH="\$HOME\/.nashmic\/bin:\$PATH"/d' "${profile}"
            # Clean trailing blank lines
            sed -i -e :a -e '/^\n*$/{$d;N;ba' -e '}' "${profile}" 2>/dev/null || true
            success "Cleaned ${profile}"
        fi
    done
}

# ── Main ───────────────────────────────────────────────────────────────────

main() {
    printf "\n"
    step "NashmiC Uninstaller - بدك تفك ارتباط؟"
    printf "\n"

    # Check if installed
    if [ ! -d "${NASHMIC_HOME}" ]; then
        warn "NashmiC is not installed at ${NASHMIC_HOME}"
        info "Nothing to remove. مش ملاقيها أصلاً"
        exit 0
    fi

    # Show what will be removed
    printf "${YELLOW}  The following will be removed:${RESET}\n"
    printf "\n"
    printf "    ${DIM}Directory:${RESET}  ${NASHMIC_HOME}/\n"

    if [ -d "${NASHMIC_HOME}" ]; then
        local size
        size=$(du -sh "${NASHMIC_HOME}" 2>/dev/null | cut -f1)
        printf "    ${DIM}Size:${RESET}       ${size}\n"
    fi

    printf "    ${DIM}Config:${RESET}     NASHMIC_ROOT and PATH entries from shell profiles\n"
    printf "\n"

    # Show what's inside
    if [ -f "${NASHMIC_HOME}/bin/mansaf" ]; then
        printf "    ${DIM}Contains:${RESET}\n"
        [ -d "${NASHMIC_HOME}/bin" ]      && printf "      • bin/mansaf (compiler)\n"
        [ -d "${NASHMIC_HOME}/runtime" ]  && printf "      • runtime/ (runtime library)\n"
        [ -d "${NASHMIC_HOME}/examples" ] && printf "      • examples/ (example programs)\n"
        [ -d "${NASHMIC_HOME}/stdlib" ]   && printf "      • stdlib/ (standard library)\n"
        [ -d "${NASHMIC_HOME}/spec" ]     && printf "      • spec/ (language spec)\n"
        printf "\n"
    fi

    # Confirmation prompt
    printf "${BOLD}  Remove NashmiC? (y/N): ${RESET}"
    read -r confirm

    if [ "${confirm}" != "y" ] && [ "${confirm}" != "Y" ]; then
        printf "\n"
        info "Cancelled. الحمدلله، ضلّت معنا"
        printf "\n"
        exit 0
    fi

    printf "\n"

    # Remove installation directory
    step "Removing ${NASHMIC_HOME}..."
    rm -rf "${NASHMIC_HOME}"
    success "Removed installation directory"

    # Clean shell profiles
    step "Cleaning shell profiles..."
    remove_shell_config

    printf "\n"
    printf "${GREEN}${BOLD}  NashmiC has been removed. الله يسهّلك وين ما رحت${RESET}\n"
    printf "\n"
    printf "  ${DIM}Restart your shell or run:${RESET}\n"
    printf "  ${CYAN}source ~/.bashrc${RESET}  ${DIM}# or source ~/.zshrc${RESET}\n"
    printf "\n"
    printf "  ${DIM}To reinstall:${RESET}\n"
    printf "  ${CYAN}curl -fsSL https://nashmic.dev/install.sh | bash${RESET}\n"
    printf "\n"
}

main "$@"
