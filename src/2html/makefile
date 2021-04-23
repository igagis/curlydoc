include prorab.mk

$(eval $(call prorab-config, ../../config))

this_name := cdoc2html

this_srcs := $(call prorab-src-dir, .)

this_libcurlydoc := $(d)../lib/out/$(c)/libcurlydoc$(dot_so)

this_cxxflags += -I$(d)../lib
this_ldlibs += $(this_libcurlydoc)

$(eval $(prorab-build-app))

$(eval $(call prorab-depend, $(this_libcurlydoc)))

$(eval $(call prorab-include, ../lib/makefile))
