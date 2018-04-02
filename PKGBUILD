# Maintainer: Alexandre Carlton
_pkgname=lightstatus
pkgname="${_pkgname}-git"
pkgver=0.1
pkgrel=1
pkgdesc="A lightweight statusline generator"
arch=('i686' 'x86_64')
url="https://github.com/AlexandreCarlton/lightstatus"
license=('GPL')
depends=('alsa-lib' 'libev' 'wireless_tools')
makedepends=('git')
provides=("${_pkgname}")
conflicts=("${_pkgname}")
source=("${pkgname}::git://github.com/AlexandreCarlton/lightstatus.git")
md5sums=('SKIP')

pkgver() {
  git -C "${pkgname}" log --pretty=format:'%h' --max-count=1
}

build() {
  make -C "${pkgname}"
}

package() {
  make INSTALL_DIR="${pkgdir}/usr" -C "${pkgname}" install
}

# vim:set ts=2 sw=2 et:
