Summary: IrTcl - Tcl module that implements the Z39.50 protocol.
Name: ir-tcl
Version: 1.5
Release: 1
Copyright: none
Group: Development/Libraries
Vendor: Index Data ApS <info@indexdata.dk>
Url: http://www.indexdata.dk/irtcl/
Source: ir-tcl-1.5.tar.gz
Requires: tcl
BuildRoot: /var/tmp/%{name}-%{version}-root
Packager: Adam Dickmeiss <adam@indexdata.dk>

%description
The IrTcl is a Tcl/Tk extension that implements the Z39.50 information
retrieval protocol. The client (origin) is supported using a number
of Tcl commands. The toolkit is shipped with an 'example' client (irclient)
as well.

%prep
%setup

%build

CFLAGS="$RPM_OPT_FLAGS" \
 ./configure --with-build-root=$RPM_BUILD_ROOT --prefix=/usr 
make CFLAGS="$RPM_OPT_FLAGS"

%install
rm -fr $RPM_BUILD_ROOT
make install

%files
%defattr(-,root,root)
%doc README LICENSE CHANGELOG
/usr/bin/irclient
/usr/lib/irtcl
/usr/lib/libirtcl.a
