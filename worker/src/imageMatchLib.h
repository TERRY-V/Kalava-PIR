/*****************************************************
*                                                    *
*	version: 1.0.0, author: Ying, data: 2014/3/6     *
*   platform: windows | linux                        *
*                                                    *
*****************************************************/

#pragma once

#include <string>
#include <vector>

#define USE_VISUALWORD

#if defined WIN32
    #if IS_IMAGEMATCHLIB
        #define LIB_API __declspec(dllexport)
    #else
        #define LIB_API __declspec(dllimport)
    #endif
#else
    #define LIB_API
#endif

LIB_API void init_lib(const char *workDir = "");
LIB_API bool set_param_bySave(const char *paramFile);
LIB_API bool set_param_byLoad(const char *paramFile);
LIB_API bool import_image(const char *trainDir = "");
LIB_API bool mask_image(const char *trainDir = "");
LIB_API bool cast_feature_bySave(const char *featureDir = "");
LIB_API bool cast_feature_byLoad(const char *featureDir = "");
#ifdef USE_VISUALWORD
LIB_API bool compute_word_bySave(const char *wordDir = "");
LIB_API bool compute_word_byLoad(const char *wordDir = "");
LIB_API bool cluster_feature2word_bySave();
LIB_API bool cluster_feature2word_byLoad();
#endif
LIB_API bool build_index_bySave(const char *indexDir = "");
LIB_API bool build_index_byLoad(const char *indexDir = "");
LIB_API bool add_train_image(const char *imageFile);
LIB_API bool delete_train_image(const char *imageFile);
LIB_API bool update_train_file();
LIB_API bool updata_feature2word();
LIB_API bool search_image_init();
LIB_API bool search_image_free();
LIB_API std::vector<std::pair<std::string, float> > search_image(const char *imageFile, const unsigned int threadId = 0);
//LIB_API float certify_geometry(const char *leftImageName, const char *rightImageName);
LIB_API void clear();
LIB_API void release_lib();
