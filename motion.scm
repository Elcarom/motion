;;; Ping @rakino in case of dependency issue.
;;;
;;; This Git repository is available as a Guix channel
;;; https://guix.gnu.org/manual/devel/en/html_node/Channels.html
;;;
;;; --8<---------------cut here---------------start------------->8---
;;; (channel
;;;   (name 'motion)
;;;   (url "https://github.com/YOUR-ORG/motion")
;;;   (branch "v5"))
;;; --8<---------------cut here---------------end--------------->8---
;;;
;;; It provides this (motion) module with the motion-git package.

(define-module (motion)
  ;; Utilities
  #:use-module (guix gexp)
  #:use-module ((guix licenses) #:prefix license:)
  #:use-module (guix packages)
  #:use-module (guix utils)
  ;; Guix origin methods
  #:use-module (guix git-download)
  ;; Guix build systems
  #:use-module (guix build-system meson)
  #:use-module (guix build-system gnu)
  ;; Guix packages
  #:use-module (gnu packages cpp)
  #:use-module (gnu packages curl)
  #:use-module (gnu packages fontutils)
  #:use-module (gnu packages freedesktop)
  #:use-module (gnu packages gl)
  #:use-module (gnu packages glib)
  #:use-module (gnu packages gnome)
  #:use-module (gnu packages gtk)
  #:use-module (gnu packages image)
  #:use-module (gnu packages jemalloc)
  #:use-module (gnu packages linux)
  #:use-module (gnu packages markup)
  #:use-module (gnu packages maths)
  #:use-module (gnu packages multiprecision)
  #:use-module (gnu packages pkg-config)
  #:use-module (gnu packages polkit)
  #:use-module (gnu packages stb)
  #:use-module (gnu packages xdisorg)
  #:use-module (gnu packages xml))

;; guix distributes stb headers under include/stb_x.h, but motion
;; expects them in include/stb/stb_x.h
(define* (wrap-stb-header stb-package name #:key deprecated?)
  (package
   (inherit stb-package)
   (arguments
    (list
     #:modules '((guix build utils))
     #:builder
     #~(begin
         (use-modules (guix build utils))
         (let ((headers-dir #$(file-append (this-package-input "stb")
                                           (if deprecated? "/deprecated" "")))
               (lib (string-join (string-split #$name #\-) "_"))
               (out #$output))
           (install-file (string-append headers-dir "/" lib ".h")
                         (string-append out "/include/stb/"))
           #t))))))
(define stb-image-resize2-wrapped
  (wrap-stb-header stb-image-resize2 "stb-image-resize2"))
(define stb-image-write-wrapped
  (wrap-stb-header stb-image-write "stb-image-write"))

(define wayland-protocols-1.48
  (package
    (inherit wayland-protocols)
    (name "wayland-protocols")
    (version "1.48")
    (source (origin
              (method git-fetch)
              (uri (git-reference
                     (url "https://gitlab.freedesktop.org/wayland/wayland-protocols")
                     (commit version)))
              (file-name (git-file-name name version))
              (sha256
               (base32
                "0zqnn7bwqzifchjhclrrcqnp39cpd3nnf6nbd9bav2hwhcx92mwy"))))))

(define source-checkout
  (local-file "." "motion-checkout"
              #:recursive? #t
              #:select?
              (or (git-predicate (current-source-directory))
                  (const #t))))

(define-public motion-git
  (package
    (name "motion-git")
    (version "latest")
    (source source-checkout)
    (build-system meson-build-system)
    (arguments
     (list #:build-type "release"
           #:phases
           #~(modify-phases %standard-phases
               (add-after 'unpack 'prepare-for-build
                 (lambda _
                   ;; For reproducibility.
                   (substitute* "meson.build"
                     (("'-march=native', '-mtune=native',") ""))
                   ;; /bin/sh doesn't exist in the build environment.
                   (substitute* "tests/process_test.cpp"
                     (("/bin/(sh)" _ cmd)
                      (which cmd))))))))
    (native-inputs
     (list pkg-config))
    (inputs
     (list cairo
           curl
           fontconfig
           freetype
           glib
           gmp
           harfbuzz
           jemalloc
           mpfr
           (librsvg-for-system)
           libqalculate
           libwebp
           libxkbcommon
           libxml2
           linux-pam
           md4c
           mesa
           nlohmann-json
           pango
           pipewire
           polkit
           sdbus-c++
           stb-image-resize2-wrapped
           stb-image-write-wrapped
           tomlplusplus
           wayland
           wayland-protocols-1.48
           wireplumber))
    (home-page #f)
    (synopsis "Wayland shell and bar")
    (description
     "Motion is a lightweight Wayland shell and bar built directly on
Wayland and OpenGL ES, with no Qt or GTK dependency.")
    (license license:expat)))

;; Also return the package at the end, so that this file can be used by
;; commands that evaluate it.  For example:
;;
;; guix build --file=motion.scm
;; guix shell --file=motion.scm
;; guix package --install-from-file=motion.scm
motion-git
