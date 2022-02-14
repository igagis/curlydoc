include prorab.mk
include prorab-test.mk

$(eval $(call prorab-config, ../../config))

this_name := unit_tests

this_srcs := $(call prorab-src-dir, src)
this_srcs += ../../src/curlydoc-html/translator_to_html.cpp

this_libcurlydoc := $(d)../../src/lib/out/$(c)/libcurlydoc$(dot_so)

this_cxxflags += -I $(d)../../src/lib
this_ldlibs += $(this_libcurlydoc) -ltst -ltreeml -lpapki

$(eval $(prorab-build-app))

$(eval $(call prorab-depend, $(prorab_this_name), $(this_libcurlydoc)))

this_test_cmd := $(prorab_this_name)
this_test_deps := $(prorab_this_name)
this_test_ld_path := ../../src/lib/out/$(c)
$(eval $(prorab-test))
