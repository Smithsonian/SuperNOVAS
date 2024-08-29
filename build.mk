
# ============================================================================
# Generic build targets and recipes for SuperNOVAS.
# 
# You can include this in your Makefile also.
# ============================================================================


# Regular object files
$(OBJ)/%.o: %.c dep/%.d $(OBJ) Makefile
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

# Share library recipe
$(LIB)/%.so.$(SO_VERSION) : | $(LIB) Makefile
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $^ -shared -fPIC -Wl,-soname,$(subst $(LIB)/,,$@) $(LD_FLAGS)

# Unversioned shared libs (for linking against)
$(LIB)/lib%.so:
	ln -sr $< $@

# Static library recipe
$(LIB)/%.a:
	ar -rc $@ $^
	ranlib $@

# Create sub-directories for build targets
dep $(OBJ) $(LIB) $(BIN) apidoc:
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
dox: README.md Doxyfile | apidoc
	@echo "   [doxygen]"
	@$(DOXYGEN)

# Automatic dependence on included header files.
.PRECIOUS: dep/%.d
dep/%.d: %.c dep
	@echo " > $@" \
	&& $(CC) $(CPPFLAGS) -I$(INC) -MM -MG $< > $@.$$$$ \
	&& sed 's|\w*\.o[ :]*| $(OBJ)/&|g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

