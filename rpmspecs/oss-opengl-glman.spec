Summary: OpenGL library man pages
Name: oss-opengl-glman
Version: 1.2
Release: 1
Copyright: GPL
Group: Development/Libraries
Source0: ftp://oss.sgi.com/www/projects/ogl-sample/download/ogl-sample.20000126.tgz
Packager: Silicon Graphics (owner-ogl-sample@oss.sgi.com)
Distribution: Red Hat
BuildRoot: /var/tmp/%{name}-buildroot

%description
GLU library for the OpenGL Development Environment

%prep
#%setup -q
#%patch -p1 -b .buildroot

%build
#make RPM_OPT_FLAGS="$RPM_OPT_FLAGS"

%install
mkdir -p $RPM_BUILD_ROOT/usr/man/man3

%clean
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(444,root,root)
#%doc README TODO COPYING ChangeLog


/usr/man/man3/glaccum.3gl
/usr/man/man3/glactivetextureARB.3gl
/usr/man/man3/glalphafunc.3gl
/usr/man/man3/glaretexturesresident.3gl
/usr/man/man3/glarrayelement.3gl
/usr/man/man3/glbegin.3gl
/usr/man/man3/glbindtexture.3gl
/usr/man/man3/glbitmap.3gl
/usr/man/man3/glblendcolor.3gl
/usr/man/man3/glblendequation.3gl
/usr/man/man3/glblendfunc.3gl
/usr/man/man3/glcalllist.3gl
/usr/man/man3/glcalllists.3gl
/usr/man/man3/glclear.3gl
/usr/man/man3/glclearaccum.3gl
/usr/man/man3/glclearcolor.3gl
/usr/man/man3/glcleardepth.3gl
/usr/man/man3/glclearindex.3gl
/usr/man/man3/glclearstencil.3gl
/usr/man/man3/glclientactivetextureARB.3gl
/usr/man/man3/glclipplane.3gl
/usr/man/man3/glcolor.3gl
/usr/man/man3/glcolormask.3gl
/usr/man/man3/glcolormaterial.3gl
/usr/man/man3/glcolorpointer.3gl
/usr/man/man3/glcolorsubtable.3gl
/usr/man/man3/glcolortable.3gl
/usr/man/man3/glcolortableparameter.3gl
/usr/man/man3/glconvolutionfilter1d.3gl
/usr/man/man3/glconvolutionfilter2d.3gl
/usr/man/man3/glconvolutionparameter.3gl
/usr/man/man3/glcopycolorsubtable.3gl
/usr/man/man3/glcopycolortable.3gl
/usr/man/man3/glcopyconvolutionfilter1d.3gl
/usr/man/man3/glcopyconvolutionfilter2d.3gl
/usr/man/man3/glcopypixels.3gl
/usr/man/man3/glcopyteximage1d.3gl
/usr/man/man3/glcopyteximage2d.3gl
/usr/man/man3/glcopytexsubimage1d.3gl
/usr/man/man3/glcopytexsubimage2d.3gl
/usr/man/man3/glcopytexsubimage3d.3gl
/usr/man/man3/glcullface.3gl
/usr/man/man3/gldeletelists.3gl
/usr/man/man3/gldeletetextures.3gl
/usr/man/man3/gldepthfunc.3gl
/usr/man/man3/gldepthmask.3gl
/usr/man/man3/gldepthrange.3gl
/usr/man/man3/gldrawarrays.3gl
/usr/man/man3/gldrawbuffer.3gl
/usr/man/man3/gldrawelements.3gl
/usr/man/man3/gldrawpixels.3gl
/usr/man/man3/gldrawrangeelements.3gl
/usr/man/man3/gledgeflag.3gl
/usr/man/man3/gledgeflagpointer.3gl
/usr/man/man3/glenable.3gl
/usr/man/man3/glenableclientstate.3gl
/usr/man/man3/glevalcoord.3gl
/usr/man/man3/glevalmesh.3gl
/usr/man/man3/glevalpoint.3gl
/usr/man/man3/glfeedbackbuffer.3gl
/usr/man/man3/glfinish.3gl
/usr/man/man3/glflush.3gl
/usr/man/man3/glfog.3gl
/usr/man/man3/glfrontface.3gl
/usr/man/man3/glfrustum.3gl
/usr/man/man3/glgenlists.3gl
/usr/man/man3/glgentextures.3gl
/usr/man/man3/glget.3gl
/usr/man/man3/glgetclipplane.3gl
/usr/man/man3/glgetcolortable.3gl
/usr/man/man3/glgetcolortableparameter.3gl
/usr/man/man3/glgetconvolutionfilter.3gl
/usr/man/man3/glgetconvolutionparameter.3gl
/usr/man/man3/glgeterror.3gl
/usr/man/man3/glgethistogram.3gl
/usr/man/man3/glgethistogramparameter.3gl
/usr/man/man3/glgetlight.3gl
/usr/man/man3/glgetmap.3gl
/usr/man/man3/glgetmaterial.3gl
/usr/man/man3/glgetminmax.3gl
/usr/man/man3/glgetminmaxparameter.3gl
/usr/man/man3/glgetpixelmap.3gl
/usr/man/man3/glgetpointerv.3gl
/usr/man/man3/glgetpolygonstipple.3gl
/usr/man/man3/glgetseparablefilter.3gl
/usr/man/man3/glgetstring.3gl
/usr/man/man3/glgettexenv.3gl
/usr/man/man3/glgettexgen.3gl
/usr/man/man3/glgetteximage.3gl
/usr/man/man3/glgettexlevelparameter.3gl
/usr/man/man3/glgettexparameter.3gl
/usr/man/man3/glhint.3gl
/usr/man/man3/glhistogram.3gl
/usr/man/man3/glindex.3gl
/usr/man/man3/glindexmask.3gl
/usr/man/man3/glindexpointer.3gl
/usr/man/man3/glinitnames.3gl
/usr/man/man3/glinterleavedarrays.3gl
/usr/man/man3/glisenabled.3gl
/usr/man/man3/glislist.3gl
/usr/man/man3/glistexture.3gl
/usr/man/man3/gllight.3gl
/usr/man/man3/gllightmodel.3gl
/usr/man/man3/gllinestipple.3gl
/usr/man/man3/gllinewidth.3gl
/usr/man/man3/gllistbase.3gl
/usr/man/man3/glloadidentity.3gl
/usr/man/man3/glloadmatrix.3gl
/usr/man/man3/glloadname.3gl
/usr/man/man3/gllogicop.3gl
/usr/man/man3/glmap1.3gl
/usr/man/man3/glmap2.3gl
/usr/man/man3/glmapgrid.3gl
/usr/man/man3/glmaterial.3gl
/usr/man/man3/glmatrixmode.3gl
/usr/man/man3/glminmax.3gl
/usr/man/man3/glmultitexcoordARB.3gl
/usr/man/man3/glmultmatrix.3gl
/usr/man/man3/glnewlist.3gl
/usr/man/man3/glnormal.3gl
/usr/man/man3/glnormalpointer.3gl
/usr/man/man3/glortho.3gl
/usr/man/man3/glpassthrough.3gl
/usr/man/man3/glpixelmap.3gl
/usr/man/man3/glpixelstore.3gl
/usr/man/man3/glpixeltransfer.3gl
/usr/man/man3/glpixelzoom.3gl
/usr/man/man3/glpointsize.3gl
/usr/man/man3/glpolygonmode.3gl
/usr/man/man3/glpolygonoffset.3gl
/usr/man/man3/glpolygonstipple.3gl
/usr/man/man3/glprioritizetextures.3gl
/usr/man/man3/glpushattrib.3gl
/usr/man/man3/glpushclientattrib.3gl
/usr/man/man3/glpushmatrix.3gl
/usr/man/man3/glpushname.3gl
/usr/man/man3/glrasterpos.3gl
/usr/man/man3/glreadbuffer.3gl
/usr/man/man3/glreadpixels.3gl
/usr/man/man3/glrect.3gl
/usr/man/man3/glrendermode.3gl
/usr/man/man3/glresethistogram.3gl
/usr/man/man3/glresetminmax.3gl
/usr/man/man3/glrotate.3gl
/usr/man/man3/glscale.3gl
/usr/man/man3/glscissor.3gl
/usr/man/man3/glselectbuffer.3gl
/usr/man/man3/glseparablefilter2d.3gl
/usr/man/man3/glshademodel.3gl
/usr/man/man3/glstencilfunc.3gl
/usr/man/man3/glstencilmask.3gl
/usr/man/man3/glstencilop.3gl
/usr/man/man3/gltexcoord.3gl
/usr/man/man3/gltexcoordpointer.3gl
/usr/man/man3/gltexenv.3gl
/usr/man/man3/gltexgen.3gl
/usr/man/man3/glteximage1d.3gl
/usr/man/man3/glteximage2d.3gl
/usr/man/man3/glteximage3d.3gl
/usr/man/man3/gltexparameter.3gl
/usr/man/man3/gltexsubimage1d.3gl
/usr/man/man3/gltexsubimage2d.3gl
/usr/man/man3/gltexsubimage3d.3gl
/usr/man/man3/gltranslate.3gl
/usr/man/man3/glvertex.3gl
/usr/man/man3/glvertexpointer.3gl
/usr/man/man3/glviewport.3gl

%changelog
