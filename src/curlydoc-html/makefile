include prorab.mk
include prorab-test.mk

$(eval $(call prorab-config, ../../config))

this_name := curlydoc-html

this_srcs := $(call prorab-src-dir, .)

this_libcurlydoc := $(d)../lib/out/$(c)/libcurlydoc$(dot_so)

this_cxxflags += -I $(d)../lib
this_ldlibs += $(this_libcurlydoc) -l clargs -l papki -l tml

$(eval $(prorab-build-app))

$(eval $(call prorab-depend, $(prorab_this_name), $(this_libcurlydoc)))

this_run_name := $(this_name)
this_test_cmd := $(d)$(this_out_dir)$(this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := $(dir $(this_libcurlydoc))
$(eval $(prorab-run))

$(eval $(call prorab-include, ../lib/makefile))
