Name:           vash
Version:        1.26.3
Release:        1%{?dist}
Summary:        Visual Assistant Shell (vash)


License:        BSD
URL:            file:///home/vsv/proj-ws/
#%{name}-%{version}.tar.gz
Source0:        %url/%{name}-%{version}.tar.gz


BuildRequires:  ncurses-devel
Requires:       ncurses

%description
Visual Assistant Shell, interactive supershell over sh/bash
features visual assistance for command line parameters, command string editing, parameterized command history, hotkey scripting, etc.

%global debug_package %{nil}

%prep
%autosetup


%build
#%configure
#make cfg
./configure
%make_build
#make


%install
#rm -rf $RPM_BUILD_ROOT
%make_install

%files
%license LICENSE
%doc DOC
/usr/bin/*
/usr/lib/*


%changelog
* Mon Jul 2 2018 Sergey Vovk <vsv>
- build with -ncurses, compatible with tgoto,tgetent from libtermcap

* Fri Aug  5 2016 Sergey Vovk <s.vovk>
- initial build for Fedora
