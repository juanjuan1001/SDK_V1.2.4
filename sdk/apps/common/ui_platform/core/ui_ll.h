#ifndef _UI_LL_H
#define _UI_LL_H

#include "ui_core.h"

int append_node(ui_img_t **head_ref, uint16_t num, pImageInfo_t pic);

void delete_node(ui_img_t **head_ref, int image_id);

void free_list(ui_img_t **head_ref);

ui_img_t *find_node_by_id(ui_img_t *head, obj_id_t id);

void node_info_dump(ui_img_t *currentNode, u8 bulkIndex);

#endif
