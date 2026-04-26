#include "ui_ll.h"
#include "ui_api.h"

//#define LOG_TAG_CONST       UI
#define LOG_TAG             "[UI_LL]"
#include "debug.h"

// 打印单向链表内容
static void dump_list(ui_img_t *node)
{
    log_info("linked list: \n");
    while (node != NULL) {
        log_info("num: %d", node->num);
        log_info("[pic info]id: 0x%x, x: %d, y: %d, w: %d, h: %d", node->pic->id, node->pic->rect.x, \
                 node->pic->rect.y, node->pic->rect.width, node->pic->rect.height);
        node = node->next;
    }
}

/* 队列--节点后插 */
int append_node(ui_img_t **head_ref, uint16_t num, pImageInfo_t pic)
{
    ui_img_t *new_node = (ui_img_t *)malloc(sizeof(ui_img_t));
    if (new_node == NULL) {
        log_info("err malloc fail");
        return -1;
    }
    new_node->img_type = 0;
    new_node->num = num;
    new_node->pic = pic;
    new_node->act_flag = 0;
    new_node->state = 0;
    new_node->jlp.rgb_result = 0;
    new_node->rect = pic->rect;
    new_node->next = NULL;

    if (*head_ref == NULL) {
        *head_ref = new_node;
        return 0;
    }

    ui_img_t *last = *head_ref;
    while (last->next != NULL) {
        last = last->next;
    }
    last->next = new_node;
    return 0;
}

void delete_node(ui_img_t **head_ref, int image_id)
{
    ui_img_t *temp = *head_ref;
    ui_img_t *prev = NULL;

    if (temp != NULL && temp->pic->id == image_id) {     //非空链表，且key可以匹配上
        *head_ref = temp->next;                 //head指针指向下一个节点
        free(temp);                             //释放上一个节点的内存
        temp = NULL;
        return;
    }
    while (temp != NULL && temp->pic->id != image_id) {  //非空链表，但key匹配不上，轮询后续节点是否key能匹配上
        prev = temp;                            //保存当前节点指针
        temp = temp->next;                      //针指向下一个节点
    }
    if (temp == NULL) {
        return;
    }
    prev->next = temp->next;
    free(temp);
    temp = NULL;
}

void free_list(ui_img_t **head_ref)
{
    ui_img_t *current = *head_ref;
    ui_img_t *next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    *head_ref = NULL;
}

ui_img_t *find_node_by_id(ui_img_t *head, obj_id_t id)
{
    ui_img_t *curNode = head;
    while (curNode != NULL) {
        if (curNode->pic->id == id) {
            break;
        }
        curNode = curNode->next;
    }
    return curNode;
}

void node_info_dump(ui_img_t *currentNode, u8 bulkIndex)
{
    u16 line = ui_get_disp_drv()->buf->buf_size / ui_get_disp_drv()->disp_width / 2;
    log_info("current Node printf[bulk index: %d, line: %d]: ", bulkIndex, line);
    log_info("current img id: 0x%x", currentNode->pic->id);
    log_info("current img act flag is: %d", currentNode->act_flag);
    log_info("current pic num: %d[total: %d]", currentNode->num, currentNode->pic->number);
    log_info("id: 0x%x, x: %d, y: %d, w: %d, h: %d", currentNode->pic->id, currentNode->pic->rect.x, \
             currentNode->pic->rect.y, currentNode->pic->rect.width, currentNode->pic->rect.height);
}
