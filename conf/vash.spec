Name:           vash
Version:        1.28.2
Release:        1%{?dist}
Summary:        Visual Assistant Shell (vash)


License:        BSD
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

%clean
rm -rf %{buildroot}


%files
#%license LICENSE
#%doc DOC
/usr/bin/*
/usr/lib/*
#/usr/share/doc/*

%changelog
* Mon Jul 15 2019 Sergey Vovk <vsv>
- Big number of changes, see docs

* Mon Jul 2 2018 Sergey Vovk <vsv>
- build with -ncurses, compatible with tgoto,tgetent from libtermcap

* Fri Aug  5 2016 Sergey Vovk <s.vovk>
- initial build for Fedora
