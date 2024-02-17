
# ============================================================================
# Generic build targets and recipes for SuperNOVAS.
# 
# You can include this in your Makefile also.
# ============================================================================


# Regular object files
obj/%.o: %.c dep/%.d obj Makefile
	$(CC) -o $@ -c $(CFLAGS) $<

# Create sub-directories for build targets
dep obj lib bin apidoc:
	mkdir $@

# Remove intermediate files locally
.PHONY: clean-local
clean-local:
	rm -rf obj dep

# Remove all locally built files, effectively restoring the repo to its 
# pristine state
.PHONY: distclean-local
distclean-local: clean-local
	rm -rf bin lib apidoc

# Remove intermediate files (general)
.PHONY: clean
clean: clean-local

# Remove intermediate files (general)
.PHONY: distclean
distclean: distclean-local

# Static code analysis using 'cppcheck'
.PHONY: check
check:
	@echo "   [check]"
	@cppcheck -I$(INC) $(CHECKOPTS) src

# Doxygen documentation (HTML and man pages) under apidocs/
.PHONY: dox
dox: README.md | apidoc
	@echo "   [doxygen]"
	@doxygen

# Automatic dependence on included header files.
.PRECIOUS: dep/%.d
dep/%.d: %.c dep
	@echo " > $@" \
	&& $(CC) -I$(INC) -MM -MG $< > $@.$$$$ \
	&& sed 's|\w*\.o[ :]*| obj/&|g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

