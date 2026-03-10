#!/usr/bin/env bash
# ============================================================================
#  NashmiC Installer - يلا نبلّش
#  Install the NashmiC programming language and mansaf compiler
#
#  Usage:
#    curl -fsSL https://nashmic.dev/install.sh | bash
#    ./install.sh
#    ./install.sh --uninstall
#
#  NashmiC - لغة برمجة أردنية 🇯🇴
# ============================================================================

set -euo pipefail

# ── Configuration ──────────────────────────────────────────────────────────

NASHMIC_REPO="https://github.com/Ziadstr/nashmic.git"
NASHMIC_HOME="${HOME}/.nashmic"
NASHMIC_BIN="${NASHMIC_HOME}/bin"
NASHMIC_RUNTIME="${NASHMIC_HOME}/runtime"
NASHMIC_EXAMPLES="${NASHMIC_HOME}/examples"
NASHMIC_STDLIB="${NASHMIC_HOME}/stdlib"
TMPDIR_INSTALL="${TMPDIR:-/tmp}/nashmic-install-$$"

# ── Colors ─────────────────────────────────────────────────────────────────

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
BOLD='\033[1m'
DIM='\033[2m'
RESET='\033[0m'

# Disable colors if not a terminal
if [ ! -t 1 ]; then
    RED='' GREEN='' YELLOW='' BLUE='' MAGENTA='' CYAN='' BOLD='' DIM='' RESET=''
fi

# ── Output Helpers ─────────────────────────────────────────────────────────

info()    { printf "${BLUE}[*]${RESET} %s\n" "$1"; }
success() { printf "${GREEN}[✓]${RESET} %s\n" "$1"; }
warn()    { printf "${YELLOW}[!]${RESET} %s\n" "$1"; }
error()   { printf "${RED}[✗]${RESET} %s\n" "$1" >&2; }
step()    { printf "${CYAN}[→]${RESET} ${BOLD}%s${RESET}\n" "$1"; }

banner() {
    printf "\n"
    printf "${MAGENTA}${BOLD}"
    printf "  ╔══════════════════════════════════════════════╗\n"
    printf "  ║                                              ║\n"
    printf "  ║   ███╗   ██╗ █████╗ ███████╗██╗  ██╗███╗███╗║\n"
    printf "  ║   ████╗  ██║██╔══██╗██╔════╝██║  ██║████████║║\n"
    printf "  ║   ██╔██╗ ██║███████║███████╗███████║██╔██╔██║║\n"
    printf "  ║   ██║╚██╗██║██╔══██║╚════██║██╔══██║██║╚═╝██║║\n"
    printf "  ║   ██║ ╚████║██║  ██║███████║██║  ██║██║   ██║║\n"
    printf "  ║   ╚═╝  ╚═══╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝   ╚═╝║\n"
    printf "  ║                                              ║\n"
    printf "  ║       لغة برمجة أردنية - NashmiC              ║\n"
    printf "  ║                                              ║\n"
    printf "  ╚══════════════════════════════════════════════╝\n"
    printf "${RESET}\n"
}

# ── Cleanup ────────────────────────────────────────────────────────────────

cleanup() {
    if [ -d "${TMPDIR_INSTALL}" ]; then
        rm -rf "${TMPDIR_INSTALL}"
    fi
}
trap cleanup EXIT

# ── Uninstall ──────────────────────────────────────────────────────────────

uninstall() {
    printf "\n"
    step "NashmiC Uninstaller - بدك تفك ارتباط؟"
    printf "\n"

    if [ ! -d "${NASHMIC_HOME}" ]; then
        warn "NashmiC is not installed at ${NASHMIC_HOME} (مش ملاقيها)"
        exit 0
    fi

    printf "${YELLOW}This will remove:${RESET}\n"
    printf "  • ${NASHMIC_HOME}/\n"
    printf "  • NASHMIC_ROOT and PATH entries from shell profiles\n"
    printf "\n"
    printf "${BOLD}Are you sure? (y/N): ${RESET}"
    read -r confirm
    if [ "${confirm}" != "y" ] && [ "${confirm}" != "Y" ]; then
        info "Cancelled. الحمدلله، ضلّت معنا"
        exit 0
    fi

    step "Removing ${NASHMIC_HOME}..."
    rm -rf "${NASHMIC_HOME}"
    success "Removed installation directory"

    remove_shell_config
    success "Cleaned up shell profiles"

    printf "\n"
    printf "${GREEN}${BOLD}  NashmiC has been removed. الله يسهّلك وين ما رحت${RESET}\n"
    printf "${DIM}  Restart your shell or run: source ~/.bashrc${RESET}\n"
    printf "\n"
    exit 0
}

# ── Shell Profile Management ──────────────────────────────────────────────

SHELL_CONFIG_MARKER="# NashmiC — added by installer"
SHELL_CONFIG_BLOCK=$(cat <<'SHELLEOF'
# NashmiC — added by installer
export NASHMIC_ROOT="$HOME/.nashmic"
export PATH="$HOME/.nashmic/bin:$PATH"
SHELLEOF
)

detect_shell_profiles() {
    local profiles=()

    # Always check bashrc if it exists or bash is the shell
    if [ -f "${HOME}/.bashrc" ]; then
        profiles+=("${HOME}/.bashrc")
    fi

    # Check for zsh
    if [ -f "${HOME}/.zshrc" ]; then
        profiles+=("${HOME}/.zshrc")
    fi

    # Check for .profile as fallback (used by sh, some systems)
    if [ -f "${HOME}/.profile" ] && [ ${#profiles[@]} -eq 0 ]; then
        profiles+=("${HOME}/.profile")
    fi

    # If nothing exists, default to bashrc
    if [ ${#profiles[@]} -eq 0 ]; then
        profiles+=("${HOME}/.bashrc")
    fi

    echo "${profiles[@]}"
}

add_shell_config() {
    local profiles
    profiles=$(detect_shell_profiles)

    for profile in ${profiles}; do
        if grep -qF "${SHELL_CONFIG_MARKER}" "${profile}" 2>/dev/null; then
            info "Shell config already in ${profile}, skipping"
        else
            printf "\n%s\n" "${SHELL_CONFIG_BLOCK}" >> "${profile}"
            success "Updated ${profile}"
        fi
    done
}

remove_shell_config() {
    local profiles
    profiles=$(detect_shell_profiles)

    for profile in ${profiles}; do
        if [ -f "${profile}" ] && grep -qF "${SHELL_CONFIG_MARKER}" "${profile}" 2>/dev/null; then
            # Remove the NashmiC config block (3 lines: marker, NASHMIC_ROOT, PATH)
            sed -i '/# NashmiC — added by installer/d' "${profile}"
            sed -i '/export NASHMIC_ROOT="\$HOME\/.nashmic"/d' "${profile}"
            sed -i '/export PATH="\$HOME\/.nashmic\/bin:\$PATH"/d' "${profile}"
            # Clean up any leftover blank lines at end of file
            sed -i -e :a -e '/^\n*$/{$d;N;ba' -e '}' "${profile}" 2>/dev/null || true
            info "Cleaned ${profile}"
        fi
    done
}

# ── Dependency Checks ─────────────────────────────────────────────────────

check_command() {
    command -v "$1" >/dev/null 2>&1
}

check_dependencies() {
    step "Checking dependencies... نشوف شو عنّا"
    local missing=()

    # Check for C compiler
    if check_command cc; then
        success "C compiler: $(cc --version 2>&1 | head -1)"
    elif check_command gcc; then
        success "C compiler: $(gcc --version 2>&1 | head -1)"
    elif check_command clang; then
        success "C compiler: $(clang --version 2>&1 | head -1)"
    else
        missing+=("C compiler (gcc or clang)")
    fi

    # Check for make
    if check_command make; then
        success "make: $(make --version 2>&1 | head -1)"
    else
        missing+=("make")
    fi

    # Check for git
    if check_command git; then
        success "git: $(git --version)"
    else
        missing+=("git")
    fi

    if [ ${#missing[@]} -gt 0 ]; then
        printf "\n"
        error "Missing required dependencies (ناقصنا أشياء):"
        for dep in "${missing[@]}"; do
            printf "  ${RED}•${RESET} %s\n" "${dep}"
        done
        printf "\n"
        info "Install them with your package manager:"
        printf "  ${DIM}Ubuntu/Debian:${RESET}  sudo apt install build-essential git\n"
        printf "  ${DIM}Fedora:${RESET}         sudo dnf install gcc make git\n"
        printf "  ${DIM}Arch:${RESET}           sudo pacman -S base-devel git\n"
        printf "  ${DIM}macOS:${RESET}          xcode-select --install && brew install git\n"
        printf "\n"
        exit 1
    fi

    printf "\n"
}

# ── Installation ───────────────────────────────────────────────────────────

clone_repo() {
    step "Downloading NashmiC... منجيب المنسف"

    mkdir -p "${TMPDIR_INSTALL}"

    if ! git clone --depth 1 "${NASHMIC_REPO}" "${TMPDIR_INSTALL}/nashmic" 2>&1; then
        error "Failed to clone repository. ما زبطت، حاول مرة ثانية"
        exit 1
    fi

    success "Source code downloaded"
    printf "\n"
}

build_compiler() {
    step "Building mansaf compiler... نطبخ المنسف"

    cd "${TMPDIR_INSTALL}/nashmic"

    if ! make all 2>&1; then
        error "Build failed. المنسف ما استوى، شوف الأخطاء فوق"
        exit 1
    fi

    if [ ! -f "build/mansaf" ]; then
        error "Build produced no binary. وين المنسف؟"
        exit 1
    fi

    success "Compiler built successfully"
    printf "\n"
}

install_files() {
    step "Installing NashmiC... نحطّها بمحلّها"

    # Create directory structure
    mkdir -p "${NASHMIC_BIN}"
    mkdir -p "${NASHMIC_RUNTIME}"
    mkdir -p "${NASHMIC_EXAMPLES}"
    mkdir -p "${NASHMIC_STDLIB}"

    local src="${TMPDIR_INSTALL}/nashmic"

    # Install the mansaf binary
    cp "${src}/build/mansaf" "${NASHMIC_BIN}/mansaf"
    chmod +x "${NASHMIC_BIN}/mansaf"
    success "Installed mansaf binary → ${NASHMIC_BIN}/mansaf"

    # Install runtime files
    if [ -d "${src}/runtime" ]; then
        cp -r "${src}/runtime/"* "${NASHMIC_RUNTIME}/"
        success "Installed runtime → ${NASHMIC_RUNTIME}/"
    fi

    # Install examples
    if [ -d "${src}/examples" ]; then
        cp -r "${src}/examples/"* "${NASHMIC_EXAMPLES}/"
        success "Installed examples → ${NASHMIC_EXAMPLES}/"
    fi

    # Install stdlib
    if [ -d "${src}/stdlib" ]; then
        cp -r "${src}/stdlib/"* "${NASHMIC_STDLIB}/"
        success "Installed stdlib → ${NASHMIC_STDLIB}/"
    fi

    # Install spec/docs for reference
    if [ -d "${src}/spec" ]; then
        mkdir -p "${NASHMIC_HOME}/spec"
        cp -r "${src}/spec/"* "${NASHMIC_HOME}/spec/"
    fi

    printf "\n"
}

configure_shell() {
    step "Configuring shell... نظبط الإعدادات"
    add_shell_config
    printf "\n"
}

verify_installation() {
    step "Verifying installation... نتأكد إنها زابطة"

    # Source the new config to test
    export NASHMIC_ROOT="${NASHMIC_HOME}"
    export PATH="${NASHMIC_BIN}:${PATH}"

    if [ -x "${NASHMIC_BIN}/mansaf" ]; then
        success "mansaf binary is executable"
    else
        error "mansaf binary not found or not executable"
        exit 1
    fi

    if [ -f "${NASHMIC_RUNTIME}/nsh_runtime.h" ]; then
        success "Runtime headers in place"
    else
        warn "Runtime headers not found, some features may not work"
    fi

    if [ -f "${NASHMIC_EXAMPLES}/marhaba.nsh" ]; then
        success "Examples installed. Try: mansaf run ~/.nashmic/examples/marhaba.nsh"
    fi

    printf "\n"
}

print_success() {
    printf "${GREEN}${BOLD}"
    printf "  ╔══════════════════════════════════════════════════════╗\n"
    printf "  ║                                                      ║\n"
    printf "  ║   🇯🇴  !NashmiC installed - مبروك، تمّ التنصيب        ║\n"
    printf "  ║                                                      ║\n"
    printf "  ╚══════════════════════════════════════════════════════╝\n"
    printf "${RESET}\n"

    printf "  ${BOLD}Get started:${RESET}\n"
    printf "\n"
    printf "  ${DIM}# Reload your shell or run:${RESET}\n"
    printf "  ${CYAN}source ~/.bashrc${RESET}  ${DIM}# or source ~/.zshrc${RESET}\n"
    printf "\n"
    printf "  ${DIM}# Run the hello world:${RESET}\n"
    printf "  ${CYAN}mansaf run ~/.nashmic/examples/marhaba.nsh${RESET}\n"
    printf "\n"
    printf "  ${DIM}# Compile to a binary:${RESET}\n"
    printf "  ${CYAN}mansaf build your_file.nsh -o program${RESET}\n"
    printf "\n"
    printf "  ${DIM}# Uninstall:${RESET}\n"
    printf "  ${CYAN}~/.nashmic/bin/install.sh --uninstall${RESET}\n"
    printf "\n"
    printf "  ${BOLD}Installation:${RESET}\n"
    printf "    ${DIM}Root:${RESET}     ${NASHMIC_HOME}\n"
    printf "    ${DIM}Binary:${RESET}   ${NASHMIC_BIN}/mansaf\n"
    printf "    ${DIM}Runtime:${RESET}  ${NASHMIC_RUNTIME}/\n"
    printf "    ${DIM}Examples:${RESET} ${NASHMIC_EXAMPLES}/\n"
    printf "\n"
    printf "  ${MAGENTA}${BOLD}يلا نكتب كود! Let's write some code!${RESET}\n"
    printf "\n"
}

# ── Main ───────────────────────────────────────────────────────────────────

main() {
    # Handle --uninstall flag
    if [ "${1:-}" = "--uninstall" ] || [ "${1:-}" = "uninstall" ]; then
        uninstall
    fi

    banner

    info "Installing NashmiC to ${NASHMIC_HOME}"
    printf "\n"

    # Check if already installed
    if [ -d "${NASHMIC_HOME}" ]; then
        warn "NashmiC is already installed at ${NASHMIC_HOME}"
        printf "${BOLD}  Reinstall? (y/N): ${RESET}"
        # If piped (non-interactive), default to yes
        if [ -t 0 ]; then
            read -r confirm
            if [ "${confirm}" != "y" ] && [ "${confirm}" != "Y" ]; then
                info "Cancelled. يلا المرة الجاي"
                exit 0
            fi
        else
            printf "y (non-interactive mode)\n"
        fi
        rm -rf "${NASHMIC_HOME}"
    fi

    check_dependencies
    clone_repo
    build_compiler
    install_files
    configure_shell
    verify_installation

    # Copy this installer for later uninstall use
    if [ -f "${TMPDIR_INSTALL}/nashmic/install.sh" ]; then
        cp "${TMPDIR_INSTALL}/nashmic/install.sh" "${NASHMIC_BIN}/install.sh"
        chmod +x "${NASHMIC_BIN}/install.sh"
    fi

    print_success
}

main "$@"
