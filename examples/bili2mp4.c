/*
 *   Copyright (C) 2023-2025 by C.H. Huang
 *   plushuang.tw@gmail.com
 *
 * sqxclib is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

/*	This program use ffmpeg to merge bilibili downloaded m4s files to mp4 files.

	How to use this program:

	1. copy directories and files from downloaded path of bilibili Android app:
	Android/data/com.bilibili.app.in/download

	2. run bili2mp4 in directory that has copy of directories and files in step 1.
	   or specify source directory in the bili2mp4's command line.

	3. bili2mp4 will parse files in downloaded directories and launch ffmpeg to merge files.
	   User can specify destination directory in the bili2mp4's command line.
 */

/*	If you want to build this file in Visual Studio, download dirent.h for MSVC from internet.
	https://github.com/tronkko/dirent
 */

// ----------------------------------------------------------------------------

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>      // MultiByteToWideChar(), WideCharToMultiByte()
#include <fcntl.h>        // _O_U16TEXT
#include <direct.h>       // _mkdir(), _wmkdir() for Windows

/*	These UTF-8 functions was copied from:
	https://gist.github.com/xebecnan/6d070c93fb69f40c3673
 */
wchar_t*
fromUTF8(
    const char* src,
    int src_length,  /* = 0 */
    int* out_length  /* = NULL */
    )
{
	if(!src)
		{ return NULL; }

	if(src_length == 0) { src_length = (int)strlen(src); }
	int length = MultiByteToWideChar(CP_UTF8, 0, src, src_length, 0, 0);
	wchar_t *output_buffer = (wchar_t*)malloc((length+1) * sizeof(wchar_t));
	if(output_buffer) {
		MultiByteToWideChar(CP_UTF8, 0, src, src_length, output_buffer, length);
		output_buffer[length] = L'\0';
	}
    if(out_length) { *out_length = length; }
	return output_buffer;
}

char*
toUTF8(
    const wchar_t* src,
    int src_length,  /* = 0 */
    int* out_length  /* = NULL */
    )
{
	if(!src)
		{ return NULL; }

	if(src_length == 0) { src_length = (int)wcslen(src); }
	int length = WideCharToMultiByte(CP_UTF8, 0, src, src_length,
			0, 0, NULL, NULL);
	char *output_buffer = (char*)malloc((length+1) * sizeof(char));
	if(output_buffer) {
		WideCharToMultiByte(CP_UTF8, 0, src, src_length,
				output_buffer, length, NULL, NULL);
		output_buffer[length] = '\0';
	}
    if(out_length) { *out_length = length; }
	return output_buffer;
}

#endif  // _WIN32 || _WIN64
// ----------------------------------------------------------------------------

#include <stdio.h>        // snprintf()

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>       // strpbrk()

#include <errno.h>
#include <dirent.h>       // struct dirent
//#include <stat.h>
#include <sys/stat.h>     // stat(), mkdir()

#include <sqxc/SqType.h>
#include <sqxc/SqxcJson.h>
#include <sqxc/SqxcValue.h>

#ifdef _MSC_VER
#define strdup       _strdup
#define snprintf     _snprintf
#endif

// return number of characters was replaced by to_char
int  str_replace_chars(char* str, const char* from_chars, int to_char)
{
	int   counts;

	for (counts = 0;  ; counts++) {
		str = strpbrk(str, from_chars);
		if (str == NULL)
			break;
		str[0] = to_char;
	}
	return counts;
}

typedef struct BiliEntry        BiliEntry;
typedef struct BiliEntryPage    BiliEntryPage;

/* This struct maps "page_data" field in entry.json */
struct BiliEntryPage
{
	int     page;
	char   *part;    // part name
	bool    has_alias;

	int     width;
	int     height;
	int     rotate;

	char   *download_title;
	char   *download_subtitle;
};

/* This struct maps fields of entry.json in bilibili download directory. */
struct BiliEntry
{
	char   *title;

	int     video_quality;                // 16
	int     total_time_milli;

	char   *quality_pithy_description;    // e.g. "360P"
	char   *owner_name;

	BiliEntryPage    page_data;
};

// ----------------------------------------------------------------------------

/* BiliEntryPage */
static const SqEntry *BiliEntryPagePointers[] = {
	&(SqEntry) {SQ_TYPE_INT,    "page",       offsetof(BiliEntryPage, page),      0},
	&(SqEntry) {SQ_TYPE_STR,    "part",       offsetof(BiliEntryPage, part),      0},
	&(SqEntry) {SQ_TYPE_BOOL,   "has_alias",  offsetof(BiliEntryPage, has_alias), 0},

	&(SqEntry) {SQ_TYPE_INT,    "width",      offsetof(BiliEntryPage, width),     0},
	&(SqEntry) {SQ_TYPE_INT,    "height",     offsetof(BiliEntryPage, height),    0},
	&(SqEntry) {SQ_TYPE_INT,    "rotate",     offsetof(BiliEntryPage, rotate),    0},

	&(SqEntry) {SQ_TYPE_STR,    "download_title",    offsetof(BiliEntryPage, download_title),      0},
	&(SqEntry) {SQ_TYPE_STR,    "download_subtitle", offsetof(BiliEntryPage, download_subtitle),   0},
};

static const SqType        typeBiliPage = SQ_TYPE_INITIALIZER(BiliEntryPage, BiliEntryPagePointers, 0);
#define SQ_TYPE_BILI_PAGE &typeBiliPage

/* BiliEntry */
static const SqEntry *BiliEntryPointers[] = {
	&(SqEntry) {SQ_TYPE_STR,    "title",               offsetof(BiliEntry, title),             0},
	&(SqEntry) {SQ_TYPE_INT,    "video_quality",       offsetof(BiliEntry, video_quality),     0},
	&(SqEntry) {SQ_TYPE_INT,    "total_time_milli",    offsetof(BiliEntry, total_time_milli),  0},

	&(SqEntry) {SQ_TYPE_STR,    "quality_pithy_description",  offsetof(BiliEntry, quality_pithy_description),  0},
	&(SqEntry) {SQ_TYPE_STR,    "owner_name",          offsetof(BiliEntry, owner_name),        0},

	&(SqEntry) {SQ_TYPE_BILI_PAGE,    "page_data",     offsetof(BiliEntry, page_data),         0},
};

static const SqType         typeBiliEntry = SQ_TYPE_INITIALIZER(BiliEntry, BiliEntryPointers, 0);
#define SQ_TYPE_BILI_ENTRY &typeBiliEntry

// ----------------------------------------------------------------------------

#define MAX_PATH_DEPTH    2

typedef struct BiliDir          BiliDir;
typedef struct Bili2Mp4         Bili2Mp4;

/* BiliDir maps directory in download folder of bilibili. */

struct BiliDir
{
	// BiliDir::entry equal NULL if there is no entry.json in this directory 
	BiliEntry  *entry;
	BiliDir    *next;
	BiliDir    *prev;
	BiliDir    *parent;
	BiliDir    *children;

	int         n_children;
	char       *title;        // copy of BiliDir::entry::title
	char       *path;
};

BiliDir *bili_dir_free(BiliDir *bili_dir)
{
	BiliDir *cur, *next;

	for (cur = bili_dir->children;  cur;  cur = next) {
		next = cur->next;
		bili_dir_free(cur);
	}

	next = bili_dir->next;
	if (bili_dir->next)
		bili_dir->next->prev = bili_dir->prev;
	if (bili_dir->prev)
		bili_dir->prev->next = bili_dir->next;

	if (bili_dir->entry) {
		// free BiliEntry
		sq_type_final_instance(SQ_TYPE_BILI_ENTRY, bili_dir->entry, false);
		free(bili_dir->entry);
	}
	free(bili_dir->title);
	free(bili_dir->path);
	free(bili_dir);

	return next;
}

/* Bili2Mp4 - bilibili downloaded files merge to mp4 files. */

struct Bili2Mp4
{
	SqxcValue   *xcvalue;
	SqxcJson    *xcjson;

	BiliDir     *last;       // the last BiliDir

#if defined(_WIN32) || defined(_WIN64)
	struct _stat fstatus;    // used by _wstat()
#else
	struct stat  fstatus;    // used by stat()
#endif
};

void  bili2mp4_init(Bili2Mp4 *b2m)
{
	b2m->xcjson  = (SqxcJson*)  sqxc_new(SQXC_INFO_JSON_PARSER);
	b2m->xcvalue = (SqxcValue*) sqxc_new(SQXC_INFO_VALUE);
	sqxc_insert((Sqxc*)b2m->xcvalue, (Sqxc*)b2m->xcjson, -1);
	b2m->last    = NULL;
}

void  bili2mp4_final(Bili2Mp4 *b2m)
{
	// free xcvalue and xcjson in Sqxc chain
	sqxc_free_chain((Sqxc*)b2m->xcvalue);

	while (b2m->last)
		b2m->last = bili_dir_free(b2m->last);
}

BiliDir *bili2mp4_add(Bili2Mp4 *b2m, BiliDir *parent, BiliEntry *bili_entry)
{
	BiliDir *bili_dir;

	bili_dir = calloc(1, sizeof(BiliDir));
	bili_dir->entry = bili_entry;

	if (b2m->last == NULL) {
		b2m->last =  bili_dir;
		return bili_dir;
	}

	if (parent == NULL) {
		bili_dir->next = b2m->last;
		b2m->last->prev = bili_dir;
		b2m->last = bili_dir;
	}
	else {
		bili_dir->next = parent->children;
		bili_dir->parent = parent;
		if (parent->children)
			parent->children->prev = bili_dir;
		parent->children = bili_dir;
		parent->n_children++;
	}
	return bili_dir;
}

BiliEntry *bili2mp4_parse(Bili2Mp4 *b2m, const char *entry_file)
{
	Sqxc       *xc;
	char       *buf;
	FILE       *file;
	size_t      fileSize;

	// open file
#if 0 // defined(_WIN32) || defined(_WIN64)
	count = strlen(entry_file);
	wchar_t *wstr = malloc(sizeof(wchar_t) * (count+1));
	mbstowcs(wstr, entry_file, count+1);
	file = _wfopen(wstr, L"r");
	free(wstr);
#elif defined(_WIN32) || defined(_WIN64)
	wchar_t *wstr = fromUTF8(entry_file, 0, NULL);
	file = _wfopen(wstr, L"r");
	free(wstr);
#else
	file = fopen(entry_file, "r");
#endif

	if (file == NULL)
		return NULL;
	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	buf = malloc(fileSize + 1);
	buf[fileSize] = 0;    // Null-terminated string
	fread(buf, 1, fileSize, file);

	// setup SqxcValue
	sqxc_value_container(b2m->xcvalue) = NULL;
	sqxc_value_element(b2m->xcvalue)  = SQ_TYPE_BILI_ENTRY;
	sqxc_value_instance(b2m->xcvalue) = NULL;

	// --- Sqxc chain ready to work ---
	sqxc_ready((Sqxc*)b2m->xcvalue, NULL);

	// Because arguments in xcvalue never used in sqxc chain,
	// I use xcvalue as arguments source here.
	xc = (Sqxc*)b2m->xcvalue;
	// send file data to SqxcJson Parser
	xc->name = NULL;
	xc->type = SQXC_TYPE_STR;
	xc->value.str = buf;
	sqxc_send_to((Sqxc*)b2m->xcjson, xc);

	// --- Sqxc chain finish work ---
	sqxc_finish((Sqxc*)b2m->xcvalue, NULL);

	free(buf);
	fclose(file);

	return b2m->xcvalue->instance;
}

int  bili2mp4_open_dir(Bili2Mp4 *b2m, const char *path, int path_depth)
{
	BiliDir   *bili_dir;
	BiliEntry *bili_entry;
	SqBuffer   buf = {0};
	size_t     buf_pathlen;

#if defined(_WIN32) || defined(_WIN64)
	_WDIR *dir;
	struct _wdirent *dirent;
	union {
		char    *c;
		wchar_t *wc;
	} str;

	str.wc = fromUTF8(path, 0, NULL);
	dir = _wopendir(str.wc);
	free(str.wc);
#else
	DIR   *dir;
	struct dirent *dirent;

	dir = opendir(path);
#endif

	if (dir == NULL) {
		switch (errno) {
		case EACCES:
			printf("Directory '%s' permission denied\n", path);
			break;
		case ENOENT:
			printf("Directory '%s' does not exist\n", path);
			break;
		case ENOTDIR:
			printf("'%s' is not a directory\n", path);
			break;
		}
		return errno;
	}

	sq_buffer_write(&buf, path);
	sq_buffer_write_c(&buf, '/');
	buf_pathlen = buf.writed;

	errno = 0;
	for (;;) {
#if defined(_WIN32) || defined(_WIN64)
		if ((dirent = _wreaddir(dir)) == NULL)
			break;
		if (wcscmp(dirent->d_name, L".") == 0 || wcscmp(dirent->d_name, L"..") == 0)
			continue;

		buf.writed = buf_pathlen;
		str.c = toUTF8(dirent->d_name, 0, NULL);
		sq_buffer_write(&buf, str.c);
		buf.mem[buf.writed] = 0;
		free(str.c);

		str.wc = fromUTF8(buf.mem, 0, NULL);
		_wstat(str.wc, &b2m->fstatus);
		free(str.wc);
#else
		if ((dirent  = readdir(dir)) == NULL)
			break;
		if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0)
			continue;

		buf.writed = buf_pathlen;
		sq_buffer_write(&buf, dirent->d_name);
		buf.mem[buf.writed] = 0;

		stat(buf.mem, &b2m->fstatus);
#endif

//		printf("%s is directory = %d\n", buf.mem, S_ISDIR(fstatus.st_mode));
		if (S_ISDIR(b2m->fstatus.st_mode) == 0)
			continue;

		// parse entry.json in child directory
		sq_buffer_write_c(&buf, '/');
		sq_buffer_write(&buf, "entry.json");
		buf.mem[buf.writed] = 0;
		bili_entry = bili2mp4_parse(b2m, buf.mem);
		if (bili_entry)
			printf("%s found\n", buf.mem);
		else {
			// If no entry.json in child directory
			printf("%s open failed.\n", buf.mem);
			// if current path depth == 1
			if (path_depth == 1)
				continue;
		}

		// add BiliDir and set current path
		buf.writed -= 11;    // - strlen("/entry.json")
		buf.mem[buf.writed] = 0;
		bili_dir = bili2mp4_add(b2m, (bili_entry == NULL) ? NULL : b2m->last, bili_entry);
		bili_dir->path = strdup(buf.mem);

		if (bili_entry) {
			if (bili_dir->parent && bili_entry->title)
				bili_dir->parent->title = strdup(bili_entry->title);
		}
		else if (path_depth < MAX_PATH_DEPTH) {
			bili2mp4_open_dir(b2m, buf.mem, path_depth +1);
		}
	}

#if defined(_WIN32) || defined(_WIN64)
	_wclosedir(dir);
#else
	closedir(dir);
#endif

	sq_buffer_final(&buf);
	return 0;
}

// return number of valid directory
int  bili2mp4_keep_valid(Bili2Mp4 *b2m)
{
	BiliDir  *cur, *next;
	int  count = 0;

	for (cur = b2m->last;  cur;  cur = next) {
		next = cur->next;
		if (cur->entry == NULL && cur->n_children == 0) {
			if (b2m->last == cur)
				b2m->last =  next;
			bili_dir_free(cur);
			continue;
		}
		count++;
	}

	return count;
}

// dest_path must be UTF-8-encoded string
void bili2mp4_output(Bili2Mp4 *b2m, BiliDir *bili_dir, const char *dest_path)
{
	SqBuffer   buf = {0};
	int        len;
	int        result;

	if (bili_dir->entry == NULL) {
		if (bili_dir->title) {
			// dest_path
			sq_buffer_write(&buf, dest_path);
			str_replace_chars(buf.mem, "\\/:*?\"<>|", '_');

			// folder separator character
			sq_buffer_write_c(&buf, '/');

			// title
			len = (int)buf.writed;
			sq_buffer_write(&buf, bili_dir->title);
			// replace invalid characters \/:*?"<>| by _ in title.
			buf.mem[buf.writed] = 0;
			str_replace_chars(buf.mem + len, "\\/:*?\"<>|", '_');

#if defined(_WIN32) || defined(_WIN64)
			wchar_t *wstr = fromUTF8(buf.mem, (int)buf.writed, NULL);
			_wmkdir(wstr);
			free(wstr);
#else
			mkdir(buf.mem, 0755);
#endif
			for (BiliDir *cur = bili_dir->children;  cur;  cur = cur->next)
				bili2mp4_output(b2m, cur, buf.mem);
		}
	}
	else {
		sq_buffer_write(&buf, "ffmpeg");
		// input video
		sq_buffer_write(&buf, " -i ");
		sq_buffer_write(&buf, bili_dir->path);
		sq_buffer_write_c(&buf, '/');
		len = snprintf(NULL, 0, "%d", bili_dir->entry->video_quality);
		sprintf(sq_buffer_alloc(&buf, len), "%d", bili_dir->entry->video_quality);
		sq_buffer_write_c(&buf, '/');
		sq_buffer_write(&buf, "video.m4s");
		// input audio
		sq_buffer_write(&buf, " -i ");
		sq_buffer_write(&buf, bili_dir->path);
		sq_buffer_write_c(&buf, '/');
		len = snprintf(NULL, 0, "%d", bili_dir->entry->video_quality);
		sprintf(sq_buffer_alloc(&buf, len), "%d", bili_dir->entry->video_quality);
		sq_buffer_write_c(&buf, '/');
		sq_buffer_write(&buf, "audio.m4s");

		// codec
		sq_buffer_write(&buf, " -codec copy ");

		// full path
		sq_buffer_write_c(&buf, '"');
		sq_buffer_write(&buf, dest_path);
		sq_buffer_write_c(&buf, '/');
		// begin of filename
		len = (int)buf.writed;
		if (bili_dir->entry->page_data.download_subtitle && bili_dir->entry->page_data.download_subtitle[0])
			sq_buffer_write(&buf, bili_dir->entry->page_data.download_subtitle);
		else
			sq_buffer_write(&buf, bili_dir->entry->page_data.part);
		sq_buffer_write(&buf, ".mp4");
		// replace invalid characters \/:*?"<>| by _ in filename.
		buf.mem[buf.writed] = 0;
		str_replace_chars(buf.mem + len, "\\/:*?\"<>|", '_');
		// end of filename
		sq_buffer_write_c(&buf, '"');
		buf.mem[buf.writed] = 0;

#if defined(_WIN32) || defined(_WIN64)
		printf("%s\n", buf.mem);
		wchar_t *wstr = fromUTF8(buf.mem, (int)buf.writed, NULL);
//		wprintf(L"%S\n", wstr);
		result = _wsystem(wstr);
		free(wstr);
#else
		printf("%s\n", buf.mem);
		result = system(buf.mem);
#endif

		if (result == -1)
			printf("error occurred while executing ffmpeg.\n");
	}

	sq_buffer_final(&buf);
}

// return number of entry.json
int  bili2mp4_print_list(Bili2Mp4 *b2m, BiliDir *cur)
{
	BiliEntry *bili_entry;
	int  count = 0;

	if (cur == NULL)
		cur  = b2m->last;
	for (;  cur;  cur = cur->next) {
		bili_entry = cur->entry;
		if (bili_entry == NULL) {
#if 0 // defined(_WIN32) || defined(_WIN64)
			wchar_t *wtitle = fromUTF8(cur->title, 0, NULL);
			wprintf(L"%s - %S\n", cur->path, wtitle);
#else
			printf("%s - %s\n", cur->path, cur->title);
#endif
		}
		else {
#if 0 // defined(_WIN32) || defined(_WIN64)
			wchar_t *wpart = fromUTF8(bili_entry->page_data.part, 0, NULL);
			wprintf(L"%s - %S\n", cur->path, wpart);
#else
			printf("%s - %s\n", cur->path, bili_entry->page_data.part);
#endif
			count++;
		}

		if (cur->children)
			count += bili2mp4_print_list(b2m, cur->children);
	}
	return count;
}

int  main(int argc, char* argv[])
{
	Bili2Mp4   *b2m = malloc(sizeof(Bili2Mp4));
	const char *srcDir  = ".";
	const char *destDir = ".";

	if (argc == 1) {
		printf("Usage:"  "\n"  "%*c", 2, ' ');
		printf("%s [%s] [%s]", "bili2mp4", "source directory", "destination directory");
		printf("\n\n");
		printf("If the user does not specify directory, the current directory is used."  "\n\n");
	}
	if (argc > 1)
		srcDir  = argv[1];
	if (argc > 2)
		destDir = argv[2];

	bili2mp4_init(b2m);

	bili2mp4_open_dir(b2m, srcDir, 0);
	bili2mp4_keep_valid(b2m);

	if (b2m->last == NULL) {
		printf("No bilibili entry found.\n");
		// free "Bili2Mp4" instance
		bili2mp4_final(b2m);
		free(b2m);
		// exit
		return EXIT_SUCCESS;
	}

#if defined(_WIN32) || defined(_WIN64)
	// change Code Page to UTF-8
	printf("\n");
	system("chcp 65001");
#endif

#if 0 // defined(_WIN32) || defined(_WIN64)
	_setmode(_fileno(stdout), _O_U16TEXT);
//	_setmode(_fileno(stdout), _O_U8TEXT);
	wprintf(L"\n" "Path - Title" "\n");
#else
	printf("\n" "Path - Title" "\n");
#endif
	bili2mp4_print_list(b2m, NULL);
	for (BiliDir *cur = b2m->last;  cur;  cur = cur->next)
		bili2mp4_output(b2m, cur, destDir);

	// free "Bili2Mp4" instance
	bili2mp4_final(b2m);
	free(b2m);

	return EXIT_SUCCESS;
}
