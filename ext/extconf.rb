require 'mkmf-rice'

$DLDFLAGS << ' -framework Security -framework CoreFoundation'
create_makefile('keychain_rice','ext')