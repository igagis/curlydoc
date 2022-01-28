include prorab.mk
include prorab-test.mk

$(eval $(call prorab-config, ../../config))

this_bin := ../../src/curlydoc-html/out/$(c)/curlydoc-html$(dot_exe)

this_test_deps := $(this_bin)
this_test_ld_path := ../../src/lib/out/$(c)

this_test_cmd := $(this_bin) --save-evaled *.cudoc
$(eval $(prorab-test))

$(eval $(call prorab-include, ../../src/curlydoc-html/makefile))
