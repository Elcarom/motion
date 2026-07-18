# Security policy

## Supported versions

Security fixes are developed against the current `main` branch and the latest tagged Motion release. The initial fork
release is `5.0.0-motion.1`.

## Reporting a vulnerability

Use GitHub's private security-advisory feature after publication. Until a repository security contact is configured,
contact the repository owner privately rather than opening a public issue. Include affected version, attack surface,
reproduction steps, expected impact, and a minimal proof of concept. Do not include real secrets or personal data.

Public issues are appropriate only after coordinated disclosure or for hardening ideas that do not reveal an exploitable
condition.

## Security model

Motion runs as the logged-in user and is part of the trusted desktop session. It handles sensitive surfaces including
clipboard history, notifications, lock-screen authentication, calendar credentials, D-Bus, compositor IPC, plugin code,
and user-configured commands. Plugins and configured shell commands execute with the user's privileges and must be
reviewed as code.

The fork adds the following controls:

- uid-owned, non-group/world-writable validation for `XDG_RUNTIME_DIR` before IPC and lock paths are placed there;
- uid-scoped `/tmp` fallback names, sanitized display names, no-follow lock creation, socket ownership/type checks, and
  mode `0600` for the IPC socket;
- private cache fallbacks scoped by uid rather than shared fixed `/tmp/motion-*` paths;
- staged, non-destructive legacy-profile copying that does not follow symlinks;
- permanent telemetry disablement and no new tracking or remote logging;
- offline mode for HTTP requests through Motion's network client.

## Hardening guidance

- Install from a trusted source and review plugins before enabling them.
- Keep configuration and state directories private to the user.
- Do not place secrets directly in hook commands or issue logs.
- Use a compositor and system services with current security updates.
- Prefer a valid `XDG_RUNTIME_DIR` created by the login manager.
- Treat clipboard history and notification history as sensitive local data.
- Use `shell.offline_mode = true` when network-backed shell features are not required.

## Out of scope

Security issues in the compositor, kernel, graphics driver, system D-Bus services, PAM stack, external commands,
third-party plugins, or upstream web services should also be reported to the responsible project.
