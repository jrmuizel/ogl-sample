Summary: OpenGL GLU library
Name: oss-opengl-glu
Version: 1.3
Release: 1
Copyright: GPL
Group: Development/Libraries
Source0: ftp://oss.sgi.com/www/projects/ogl-sample/download/ogl-sample.20000126.tgz
Packager: Silicon Graphics (owner-ogl-sample@oss.sgi.com)
Distribution: Red Hat
BuildRoot: /var/tmp/%{name}-buildroot
AutoReqProv: no
Requires: ld-linux.so.2 libc.so.6 libGL.so libm.so.6 libstdc++-libc6.1-1.so.2 libm.so.6(GLIBC_2.0)

%description
GLU library for the OpenGL Development Environment

%prep
#%setup -q
#%patch -p1 -b .buildroot

%build
#make RPM_OPT_FLAGS="$RPM_OPT_FLAGS"

%install
mkdir -p $RPM_BUILD_ROOT/usr/lib
mkdir -p $RPM_BUILD_ROOT/usr/man/man3
mkdir -p $RPM_BUILD_ROOT/usr/include/GL

rm -rf $RPM_BUILD_ROOT/usr/lib/libGLU.so
strip $RPM_BUILD_ROOT/usr/lib/libGLU.so.1
ln -s -f $RPM_BUILD_ROOT/usr/lib/libGLU.so.1 $RPM_BUILD_ROOT/usr/lib/libGLU.so

%postun
/sbin/ldconfig

%post
/sbin/ldconfig

%clean
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
#%doc README TODO COPYING ChangeLog

%attr(-, root, root) %dir /usr/include/GL

%attr(755, root, root) /usr/lib/libGLU.so.1
%attr(-, root, root) /usr/lib/libGLU.so
%attr(444, root, root) /usr/include/GL/glu.h

%attr(444, root, root) /usr/man/man3/glubegincurve.3gl
%attr(444, root, root) /usr/man/man3/glubeginpolygon.3gl
%attr(444, root, root) /usr/man/man3/glubeginsurface.3gl
%attr(444, root, root) /usr/man/man3/glubegintrim.3gl
%attr(444, root, root) /usr/man/man3/glubuild1dmipmaplevels.3gl
%attr(444, root, root) /usr/man/man3/glubuild1dmipmaps.3gl
%attr(444, root, root) /usr/man/man3/glubuild2dmipmaplevels.3gl
%attr(444, root, root) /usr/man/man3/glubuild2dmipmaps.3gl
%attr(444, root, root) /usr/man/man3/glubuild3dmipmaplevels.3gl
%attr(444, root, root) /usr/man/man3/glubuild3dmipmaps.3gl
%attr(444, root, root) /usr/man/man3/glucheckextension.3gl
%attr(444, root, root) /usr/man/man3/glucylinder.3gl
%attr(444, root, root) /usr/man/man3/gludeletenurbsrenderer.3gl
%attr(444, root, root) /usr/man/man3/gludeletequadric.3gl
%attr(444, root, root) /usr/man/man3/gludeletetess.3gl
%attr(444, root, root) /usr/man/man3/gludisk.3gl
%attr(444, root, root) /usr/man/man3/gluerrorstring.3gl
%attr(444, root, root) /usr/man/man3/glugetnurbsproperty.3gl
%attr(444, root, root) /usr/man/man3/glugetstring.3gl
%attr(444, root, root) /usr/man/man3/glugettessproperty.3gl
%attr(444, root, root) /usr/man/man3/gluloadsamplingmatrices.3gl
%attr(444, root, root) /usr/man/man3/glulookat.3gl
%attr(444, root, root) /usr/man/man3/glunewnurbsrenderer.3gl
%attr(444, root, root) /usr/man/man3/glunewquadric.3gl
%attr(444, root, root) /usr/man/man3/glunewtess.3gl
%attr(444, root, root) /usr/man/man3/glunextcontour.3gl
%attr(444, root, root) /usr/man/man3/glunurbscallback.3gl
%attr(444, root, root) /usr/man/man3/glunurbscallbackdata.3gl
%attr(444, root, root) /usr/man/man3/glunurbscallbackdataext.3gl
%attr(444, root, root) /usr/man/man3/glunurbscurve.3gl
%attr(444, root, root) /usr/man/man3/glunurbsproperty.3gl
%attr(444, root, root) /usr/man/man3/glunurbssurface.3gl
%attr(444, root, root) /usr/man/man3/gluortho2d.3gl
%attr(444, root, root) /usr/man/man3/glupartialdisk.3gl
%attr(444, root, root) /usr/man/man3/gluperspective.3gl
%attr(444, root, root) /usr/man/man3/glupickmatrix.3gl
%attr(444, root, root) /usr/man/man3/gluproject.3gl
%attr(444, root, root) /usr/man/man3/glupwlcurve.3gl
%attr(444, root, root) /usr/man/man3/gluquadriccallback.3gl
%attr(444, root, root) /usr/man/man3/gluquadricdrawstyle.3gl
%attr(444, root, root) /usr/man/man3/gluquadricnormals.3gl
%attr(444, root, root) /usr/man/man3/gluquadricorientation.3gl
%attr(444, root, root) /usr/man/man3/gluquadrictexture.3gl
%attr(444, root, root) /usr/man/man3/gluscaleimage.3gl
%attr(444, root, root) /usr/man/man3/glusphere.3gl
%attr(444, root, root) /usr/man/man3/glutessbegincontour.3gl
%attr(444, root, root) /usr/man/man3/glutessbeginpolygon.3gl
%attr(444, root, root) /usr/man/man3/glutesscallback.3gl
%attr(444, root, root) /usr/man/man3/glutessendpolygon.3gl
%attr(444, root, root) /usr/man/man3/glutessnormal.3gl
%attr(444, root, root) /usr/man/man3/glutessproperty.3gl
%attr(444, root, root) /usr/man/man3/glutessvertex.3gl
%attr(444, root, root) /usr/man/man3/gluunproject.3gl
%attr(444, root, root) /usr/man/man3/gluunproject4.3gl

%changelog
