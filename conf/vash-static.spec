Name:           vash
Version:
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

%define source_date_epoch_from_changelog   0
%define debug_package %{nil}

%prep
#%autosetupdpkg --print-architecture
%setup -q

%build
#%%make_build
./configure --static
make

%install
#rm -rf $RPM_BUILD_ROOT
%make_install
mkdir -p %{buildroot}/etc/vhset
mkdir -p %{buildroot}/usr/share/doc/%{name}
mkdir -p %{buildroot}/%{_mandir}/man1/
cp -rd ./etc/vhset/* %{buildroot}/etc/vhset/
cd %{buildroot}/etc/vhset; ln -sf xterm-256color-std xterm-256color

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
