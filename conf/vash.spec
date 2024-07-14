Name:           vash
Version:
#Release:        1%%{dist}
Release:        1
Summary:        Visual Assistant Shell (vash)

License:        GPL and MIT
#URL:            file:///home/vsv/proj-ws/
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  ncurses-devel
#Requires:       ncurses

%description
Visual Assistant Shell, interactive supershell over sh/bash.
Features visual assistance for typing command line parameters, editing a command string, parameterized command history, hotkey scripting, etc.

%define debug_package %{nil}

%prep
#%autosetup
%setup -q

%build
#%%configure
#make cfg
#./configure
#%%make_build
make config
make

%install
#rm -rf $RPM_BUILD_ROOT
%make_install
mkdir -p %{buildroot}/etc/vhset
mkdir -p %{buildroot}/usr/share/doc/%{name}
mkdir -p %{buildroot}/%{_mandir}/man1/
cp -rd ./etc/vhset/* %{buildroot}/etc/vhset/

%clean
rm -rf %{buildroot}

%files
/usr/bin/*
/usr/lib/*
/usr/share/doc/*
%{_mandir}/man1/*
%dir /etc/vhset
%config(noreplace) /etc/vhset/*

%changelog
* Fri Aug  5 2016 Sergey Vovk <s.vovk>
- initial build for Fedora
