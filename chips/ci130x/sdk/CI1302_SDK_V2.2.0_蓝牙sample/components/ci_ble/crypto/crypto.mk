
CRYPTO_BASE_DIR = $(EXE)/src/crypto

#$(CRYPTO_BASE_DIR)/rijndael.c
#$(CRYPTO_BASE_DIR)/aes128_enc.c
CRYPTO_SOURCES = \
    $(CRYPTO_BASE_DIR)/aes_ccm_ble.c \
    $(CRYPTO_BASE_DIR)/tiny_aes128.c

CRYPTO_INCLUDES = \
    $(CRYPTO_BASE_DIR) \
