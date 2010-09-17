# 
win32 {
 LIBNAME=dll
}
!win32 {
 LIBNAME=a
}

# This function sets up the dependencies for libraries that are built with
# this project.  Specify the libraries you need to depend on in the variable
# DEPENDENCY_LIBRARIES and this will add
for(dep, DEPENDENCY_LIBRARIES) {
    #message($$TARGET depends on $$dep ($${DESTDIR}/$${dep}.$${LIBNAME}))
    #LIBS += $${DESTDIR}/lib$${dep}.$${LIBNAME}
    PRE_TARGETDEPS += ../$${dep}/lib$${dep}.$${LIBNAME}
}

