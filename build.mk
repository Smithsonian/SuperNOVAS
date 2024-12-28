
# ============================================================================
# Generic build targets and recipes for SuperNOVAS.
# 
# You can include this in your Makefile also.
# ============================================================================


# Regular object files
$(OBJ)/%.o: $(SRC)/%.c $(OBJ) Makefile
	$(CC) -o $@ -c $(CPPFLAGS) $(CFLAGS) $<

# Share library recipe
$(LIB)/%.so.$(SO_VERSION):
	@$(MAKE) $(LIB)
	$(CC) -o $@ $(CPPFLAGS) $(CFLAGS) $^ -shared -fPIC -Wl,-soname,$(subst $(LIB)/,,$@) $(LDFLAGS)

# Unversioned shared libs (for linking against)
$(LIB)/lib%.so:
	@rm -f $@
	ln -sr $< $@

# Static library recipe
$(LIB)/%.a:
	@$(MAKE) $(LIB)
	ar -rc $@ $^
	ranlib $@

# Create sub-directories for build targets
$(OBJ) $(LIB) $(BIN) apidoc:
	mkdir -p $@

# Remove intermediate files locally
.PHONY: clean-local
clean-local:
	rm -rf obj

# Remove all locally built files, effectively restoring the repo to its 
# pristine state
.PHONY: distclean-local
distclean-local: clean-local
	rm -rf bin lib apidoc infer-out

# Remove intermediate files (general)
.PHONY: clean
clean: clean-local

# Remove intermediate files (general)
.PHONY: distclean
distclean: distclean-local

# Static code analysis using 'cppcheck'
.PHONY: analyze
analyze:
	@echo "   [analyze]"
	@cppcheck $(CPPFLAGS) $(CHECKOPTS) $(SRC)

# Static code analysis viacat Facebook's infer
.PHONY: infer
infer: clean
	infer run -- $(MAKE) shared

# Doxygen documentation (HTML and man pages) under apidocs/
.PHONY: dox
dox: README.md Doxyfile apidoc $(SRC) $(INC)
	@echo "   [doxygen]"
	@$(DOXYGEN)


