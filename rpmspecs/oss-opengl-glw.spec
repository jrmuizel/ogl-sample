Summary: OpenGL GLw library
Name: oss-opengl-glw
Version: 1.0
Release: 1
Copyright: GPL
Group: Development/Libraries
Source0: ftp://oss.sgi.com/www/projects/ogl-sample/download/ogl-sample.20000126.tgz
Packager: Silicon Graphics (owner-ogl-sample@oss.sgi.com)
Distribution: Red Hat
BuildRoot: /var/tmp/%{name}-buildroot

%description
GLw library for the OpenGL Development Environment

%prep
#%setup -q
#%patch -p1 -b .buildroot

%build
#make RPM_OPT_FLAGS="$RPM_OPT_FLAGS"

%install
mkdir -p $RPM_BUILD_ROOT/usr/lib
mkdir -p $RPM_BUILD_ROOT/usr/man/man3
mkdir -p $RPM_BUILD_ROOT/usr/include/X11/GLw

%postun
/sbin/ldconfig

%post
/sbin/ldconfig

%clean
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
#%doc README TODO COPYING ChangeLog

%attr(644, root, root) /usr/lib/libGLw.a
%attr(444, root, root) /usr/include/X11/GLw/GLwDrawA.h
%attr(444, root, root) /usr/include/X11/GLw/GLwMDrawA.h

%attr(444, root, root) /usr/man/man3/glwcreatemdrawingarea.3gl
%attr(444, root, root) /usr/man/man3/glwdrawingarea.3gl
%attr(444, root, root) /usr/man/man3/glwdrawingareamakecurrent.3gl
%attr(444, root, root) /usr/man/man3/glwdrawingareaswapbuffers.3gl

%changelog
