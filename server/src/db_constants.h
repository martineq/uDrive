// Database Path
#define DB_PATH "/tmp/testdb"

// DataHandler exit status
#define DH_STATUS_OK 1
#define DH_STATUS_USER_ALREADY_EXISTS 2
#define DH_STATUS_DATABASE_ERROR 3
#define DH_STATUS_KEY_NOT_FOUND 4

// DataHandler deleted_status
#define DH_DELETED_STATUS_EXISTS "1"
#define DH_DELETED_STATUS_ERASED "2"

// DataHandler prefixes and suffixes for key construction
#define PREFIX_USER "user_"
#define PREFIX_DIR "dir_"
#define PREFIX_FILE "file_"
#define PREFIX_INDEX_USER_ID_FROM_USER_EMAIL "index_user_id_from_user_email_"
#define PREFIX_TICKET_LAST "ticket_last_"

#define SUFFIX_EMAIL "_email"
#define SUFFIX_PASSWORD "_password"
#define SUFFIX_TOKEN "_token"
#define SUFFIX_NAME "_name"
#define SUFFIX_LOCATION "_location"
#define SUFFIX_AVATAR_FILE "_avatar_file"
#define SUFFIX_DIR_ROOT "_dir_root"
#define SUFFIX_SHARED_FILES "_shared_files"
#define SUFFIX_DATE_LAST_MOD "_date_last_mod"
#define SUFFIX_USER_LAST_MOD "_user_last_mod"
#define SUFFIX_TAGS "_tags"
#define SUFFIX_OWNER "_owner"
#define SUFFIX_PARENT_DIRECTORY "_parent_directory"
#define SUFFIX_FILES_CONTAINED "_files_contained"
#define SUFFIX_DIRECTORIES_CONTAINED "_directories_contained"
#define SUFFIX_EXTENSION "_extension"
#define SUFFIX_SIZE "_size"
#define SUFFIX_DELETED_STATUS "_deleted_status"
#define SUFFIX_FILE_REVISION "_file_revision"
#define SUFFIX_USERS_SHARED "_users_shared"
#define SUFFIX_USER_ID "user_id"
#define SUFFIX_DIR_ID "dir_id"
#define SUFFIX_FILE_ID "file_id"

// DataHandler labels
#define LABEL_ROOT "root"
#define LABEL_EMPTY_STRING ""
#define LABEL_NO_PARENT "no_parent"
