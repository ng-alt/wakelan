Summary: wakelan - send a wake-on-lan packet
Name: wakelan
Version: 1.1
Release: 1
Copyright: GPL
Group: Networking/Utilities
Source: ftp://sunsite.unc.edu/pub/Linux/system/Network/misc/wakelan-1.1.tar.gz
Buildroot: /tmp/wakelan-root

%description
WakeLan sends a properly formatted UDP packet across the network which will
cause a wake-on-lan enabled computer to power on.

%changelog
* Sat Aug 29 1998 Christopher Chan-Nui <channui+wakelan@freeware.tiny.org>

- Initial SPEC file

%prep
%setup

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/man/man1
mkdir -p $RPM_BUILD_ROOT/usr/bin
make prefix=$RPM_BUILD_ROOT/usr install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%doc README COPYING
/usr/bin/*
/usr/man/man1/*
