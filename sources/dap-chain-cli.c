#include "dap_chain.h"
#include "dap_chain_mine.h"
#include "dap_common.h"
#include "dap_config.h"
#include "dap_enc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


#define LOG_TAG "dap-chain-cli"

FILE* my_file_to_wite;

void init() {
    dap_common_init(NULL);
    dap_config_init(NULL);
    dap_enc_init();
    dap_chain_init();
}

void print_help() {}

void make_new_block(){

}

void sing_handler(int signum){
    if (signum == 2){
        fclose(my_file_to_wite);
        exit(1);
    }

}

int main(int argc, const char *argv[]) {
    log_it(L_INFO, "dap-chain-cli version 0.0.1 \n");
    signal(SIGINT, sing_handler);
    if (argc > 1) {
        init();
        char *l_config_name = strdup(argv[1]);
        dap_config_t *l_config = dap_config_open(l_config_name);
        if (l_config) {
            const char *l_chain_name =
                    dap_config_get_item_str(l_config, "general", "name");
            const char *l_chain_file_storage =
                    dap_config_get_item_str(l_config, "general", "file_storage");
            const char *l_chain_file_cache =
                    dap_config_get_item_str(l_config, "general", "file_cache");

            if (l_chain_name) {
                dap_chain_t *l_chain =
                        dap_chain_open(l_chain_file_storage, l_chain_file_cache);

                my_file_to_wite= fopen("my_block_test.txt", "w");// тестовый файл создам если есть добавим
                if (my_file_to_wite==NULL){
                    log_it(L_INFO, "Everything is lost! File not opened!");
                    exit(1);

                }


                if (l_chain) {
                    if (argc > 2) {
                        if (strcmp(argv[2], "block") == 0) {
                            if (argc > 3) {
                                if (strcmp(argv[3], "new") == 0) {//елси блон новый
                                    log_it(L_INFO, "Create new block");//то пишем "блок новый"
                                    dap_chain_block_cache_t *l_block_cache =
                                            dap_chain_allocate_next_block(l_chain);//размещаем новый блок
                                    for (int i= DAP_CHAIN_HASH_SIZE; i>1; i--){
                                       // fputc(1, my_file_to_wite);
                                        fputc(l_block_cache->block_hash.data[i-1], my_file_to_wite);
                                    }
                                    if (dap_chain_mine_block(l_block_cache, false, 0) == 0) {//майним
                                        char *l_hash_str =
                                                dap_chain_hash_to_str_new(&l_block_cache->block_hash); //че-то хеш в строку переводим
                                        log_it(L_INFO, "Block mined with hash %s ", l_hash_str);


                                      //  fclose(my_file_to_wite);

                                        dap_chain_block_cache_dump(l_block_cache);//дамп че-то
                                        DAP_DELETE(l_hash_str);//хеш стирает

                                        while (true){
                                                dap_chain_block_new(&l_block_cache);
                                                l_block_cache =
                                                        dap_chain_allocate_next_block(l_chain);//размещаем новый блок
                                                if (dap_chain_mine_block(l_block_cache, false, 0) ==0){
                                                        l_hash_str =
                                                             dap_chain_hash_to_str_new(&l_block_cache->block_hash); //че-то хеш в строку переводим
                                                        for (int i= DAP_CHAIN_HASH_SIZE; i>1; i--){
                                                            //fputc(1, my_file_to_wite);
                                                            fputc(l_block_cache->block_hash.data[i-1], my_file_to_wite);
                                                        }
                                                        log_it(L_INFO, "Block mined eith hash %s ", l_hash_str);
                                                        dap_chain_block_cache_dump(l_block_cache);//дамп че-то
                                                        DAP_DELETE(l_hash_str);//хеш стирает
                                                }

                                        }
                                    }
                                }
                            } else   {
                                log_it(L_CRITICAL, "Command 'block' need to be specified. "
                                                   "Variant: 'new' 'show' ");
                            }
                        } else if (strcmp(argv[2], "wallet") == 0) {
                            if (argc > 3) {
                                if (strcmp(argv[3], "create") == 0) {
                                    log_it(L_INFO, "Create blochain wallet");
                                }
                            } else {
                                log_it(L_CRITICAL, "Command 'wallet' need to be specified. "
                                                   "Variant: 'new' 'show' ");
                            }
                        }
                    } else {
                        log_it(L_INFO, "Information about '%s'", l_chain_name);
                        dap_chain_info_dump_log(l_chain);
                    }
                } else {
                    log_it(L_CRITICAL, "Can't open dapchain %s", l_chain_name);
                }
            } else {
                log_it(L_CRITICAL,
                       "Can't find dapchain name in config section 'general'");
            }
        } else {
            log_it(L_CRITICAL, "Can't open config name %s", l_config_name);
        }
        free(l_config_name);
    }
    return 0;
}
