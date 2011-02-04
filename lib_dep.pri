# 
win32 {
 LIB_EXT=a
}
!win32 {
 LIB_EXT=a
}

# This function sets up the dependencies for libraries that are built with
# this project.  Specify the libraries you need to depend on in the variable
# DEPENDENCY_LIBRARIES and this will add
for(dep, DEPENDENCY_LIBRARIES) {
    #message($$TARGET depends on $$dep ($${DESTDIR}/$${dep}.$${LIBNAME}))
    #LIBS += $${DESTDIR}/lib$${dep}.$${LIBNAME}
    LIB_NAME = $${dep}
    #remove the path of the lib
    LIB_NAME ~= s,.*/,
    PRE_TARGETDEPS += $${ROOT_PWD}$${dep}/lib$${LIB_NAME}.$${LIB_EXT}
}

