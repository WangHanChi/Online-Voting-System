#pragma once

/*
 * welcome window, this ascii art is created from this website
 * https://zh-tw.rakko.tools/tools/68/
 */
// clang-format off
char banner[] = "\e[36;01m ==============================================\e[0m\n"
                " #     # ####### ####### ### #     #  #####\n"
                " #     # #     #    #     #  ##    # #     #\n"
                " #     # #     #    #     #  # #   # #\n"
                " #     # #     #    #     #  #  #  # #  ####\n"
                "  #   #  #     #    #     #  #   # # #     #\n"
                "   # #   #     #    #     #  #    ## #     #\n"
                "    #    #######    #    ### #     #  #####\n"
                "\e[36;01m ==============================================\e[0m\n"
                "\n This is a simple online voting system!\n\n";
// clang-format on

/*
 * first level menu, following the design from this note
 * https://hackmd.io/@wanghanchi/CN2023_HW
 */
// clang-format off
char first_level_menu[] = "Please enter the command # as following :\n"
                          " (1) < Create a vote >\n"
                          " (2) < View votes in progress >\n"
                          " (3) < View voting results >\n"
                          " (4) < Log out > \n\n";
// clang-format on