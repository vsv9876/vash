Name:           vash
Version:        2.0.12
Release:        1%{dist}
Summary:        Visual Assistant Shell (vash)

License:        MIT and BSD
#URL:            file:///home/vsv/proj-ws/
#BuildRoot: %{_tmppath}/%{name}-root
#%{name}-%{version}.tar.gz
#Source0:        %url/%{name}-%{version}.tar.gz
Source0:        %{name}-%{version}.tar.gz


BuildRequires:  ncurses-devel
Requires:       ncurses

%description
Visual Assistant Shell, interactive supershell over sh/bash
features visual assistance for command line parameters, command string editing, parameterized command history, hotkey scripting, etc.

%define debug_package %{nil}


%prep
#%autosetup
%setup -q

%build
#%configure
#make cfg
./configure
#%make_build
make

%install
#rm -rf $RPM_BUILD_ROOT
%make_install
mkdir -p %{buildroot}/etc/vhset
mkdir -p %{buildroot}/usr/share/doc/%{name}
# install -m 664 ./etc/vhset/xterm %{buildroot}/etc/vhset/xterm
# install -m 664 ./etc/vhset/linux %{buildroot}/etc/vhset/linux
# install -m 664 ./etc/vhset/screen %{buildroot}/etc/vhset/screen
install -m 664 ./etc/vhset/* %{buildroot}/etc/vhset/
install -m 664 ./LICENSE %{buildroot}/usr/share/doc/%{name}/

%clean
rm -rf %{buildroot}


%files
#%license LICENSE
#%doc DOC
/usr/bin/*
/usr/lib/*
/usr/share/doc/*
%dir /etc/vhset
#%%config(noreplace) /etc/vhset/xterm
#%%config(noreplace) /etc/vhset/linux
#%%config(noreplace) /etc/vhset/screen
%config(noreplace) /etc/vhset/*

%changelog
* Mon Jul 15 2019 Sergey Vovk <vsv>
- Big number of changes, see docs

* Mon Jul 2 2018 Sergey Vovk <vsv>
- build with -ncurses, compatible with tgoto,tgetent from libtermcap

* Fri Aug  5 2016 Sergey Vovk <s.vovk>
- initial build for Fedora
