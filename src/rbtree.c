#include "rbtree.h"

void test_vlr(rbtree *t, node_t *now) {
    if (now == t->nil)
        return;

    printf("%d(%d) is %s\n", now->key, now == t->root ? 777 : now->parent->key, now->color == RBTREE_BLACK ? "BLACK" : "RED");

    if (now->left != t->nil)
        test_vlr(t, now->left);
    if(now->right != t->nil)
        test_vlr(t, now->right);
}

// 1-1. 메모리 : 트리 생성 
rbtree* new_rbtree(void){
    rbtree* t = (rbtree*)calloc(1, sizeof(rbtree));

    // NIL 노드 = real_nil
    node_t* real_nil = (node_t*)calloc(1, sizeof(node_t));
    real_nil->color = RBTREE_BLACK;
    real_nil->left = real_nil;
    real_nil->right = real_nil;

    t->nil = real_nil;
    t->root = real_nil;
    return t;
}

// 1-2. 메모리 : 트리 삭제 (1) - 트리
void delete_rbtree(rbtree* t){
    node_t* temp = t->root;

    delete_node(temp, t->nil);

    free(t->nil); // real_nil 메모리 반환
    free(t);      
}

// 1-2. 메모리 : 트리 삭제 (2) - 노드
void delete_node(node_t* del, node_t* nil){ 
    if(del->left != nil) {  
        delete_node(del->left, nil);
    }
    if(del->right != nil) {
        delete_node(del->right, nil);
    }
    if (del != nil) // ★★★ -> if 조건 안 걸어도 그냥 할 수 있나..?
        free(del);
}

// 2-1. 노드 삽입
node_t* rbtree_insert(rbtree* t, const key_t key){
    
    node_t* y = t->nil;    // y = 새 노드의 부모 (위치)
    node_t* pos = t->root; // pos = 새 노드의 위치

    node_t* new = (node_t*)calloc(1, sizeof(node_t)); // new = 새 노드
    new->key = key;

    // new의 위치를 찾는다.
    while(pos != t->nil){
        y = pos;
        if(new->key < pos->key){
            pos = pos->left;
        }
        else{
            pos = pos->right;
        }
    }

    new->parent = y;    // new에 부모를 연결한다.

    // 부모에 new를 연결한다.
    if(y == t->nil){    
        t->root = new;
    }
    else if (new->key < y->key){
        y->left = new;
    }
    else {
        y->right = new;
    }
 
    // (new를 삽입한 후) 남은 정보들을 업데이트 한다.
    new->left = new->right = t->nil;
    new->color = RBTREE_RED;
    rb_insert_fixup(new, t); // 노드 불균형 복구

    return new;
}

// 2-2. 노드 삽입 fixup
void rb_insert_fixup(node_t* z, rbtree* t){
    node_t* p; // 부모
    node_t* g; // 조부모
    node_t* s; // 삼촌
    node_t* now = z;

    // <4번> 속성 해결
    while(now->parent->color == RBTREE_RED){
        p = now->parent;
        g = p->parent;

        if(now->parent == now->parent->parent->left){  // (1) 삼촌이 '오른쪽' 서브 트리인 경우 -> z와 p는 왼쪽
            s = now->parent->parent->right;

            // case 1 : z의 삼촌 s가 RED인 경우 -> RECOLORING
            if(s->color == RBTREE_RED){
                now->parent->color = s->color = RBTREE_BLACK;
                now->parent->parent->color = RBTREE_RED;
                now = now->parent->parent;
            }
            // case 2 : z의 삼촌 s가 BLACK인 경우 -> RESTRUCTURING
            else {
                // case 2-1 : z가 부모 p의 오른쪽에 있는 경우 -> left 회전 + case 2-2 진행
                if(now == now->parent->right){
                    // left_rotate 인자로 넘길 z는 부모 p의 위치 (내려갈 거 기준)
                    now = now->parent;
                    left_rotate(t, now);
                }
                // case 2-2 : z - p - g가 왼쪽에 있는 경우 -> right 회전
                now->parent->color = RBTREE_BLACK;
                now->parent->parent->color = RBTREE_RED;
                // right_rotate 인자로 넘길 z는 조부모 g의 위치 (내려갈 거 기준)
                right_rotate(t, now->parent->parent);
            }
        
        }
        else{  // (2) 삼촌이 '왼쪽' 서브 트리인 경우 -> z와 p는 오른쪽
            s = now->parent->parent->left;

            // case 1 : z의 삼촌 s가 RED인 경우 -> RECOLORING
            if(s->color == RBTREE_RED){
                now->parent->color = s->color = RBTREE_BLACK;
                now->parent->parent->color = RBTREE_RED;
                now = now->parent->parent;
            }
            // case 2 : z의 삼촌 s가 BLACK인 경우 -> RESTRUCTURING
            else {
                // case 2-1 : z가 부모 p의 오른쪽에 있는 경우
                if(now == now->parent->left){
                    now = now->parent;
                    right_rotate(t, now);
                }
                // case 2-2 : z가 부모 p의 왼쪽에 있는 경우
                now->parent->color = RBTREE_BLACK;
                now->parent->parent->color = RBTREE_RED;
                left_rotate(t, now->parent->parent);
            }
        }
    }
    // 2번 속성 해결
    t->root->color = RBTREE_BLACK;
}


// 2-3. 왼쪽 회전
void left_rotate(rbtree* t, node_t* x){
    node_t* y = x->right;
    x->right = y->left;

    if(y->left != t->nil)
        y->left->parent = x;
    
    y->parent = x->parent;

    if(x->parent == t->nil){
        t->root = y;
    }
    else if(x == x->parent->left){
        x->parent->left = y;
    }
    else{
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}

// 2-4. 오른쪽 회전
void right_rotate(rbtree* t, node_t* x){
    node_t* y = x->left;
    x->left = y->right;

    if(y->right != t->nil){
        y->right->parent = x;
    }

    y->parent = x->parent;

    if(x->parent == t->nil){
        t->root = y;
    }
    else if(x == x->parent->left){
        x->parent->left = y;
    }
    else{
        x->parent->right = y;
    }

    y->right = x;
    x->parent = y;

}

// 3-1. 노드 탐색 (key 노드)
node_t* rbtree_find(const rbtree* t, const key_t key){
    node_t* find = t->root;

    if(find == t->nil){
        return NULL;
    }
    
    while(find != t->nil){ // 리프노드까지 본다.
        if(key == find->key){
            return find;
        }
        else if(key < find->key){
            find = find->left;
        }
        else{ // (key > find->key)
            find = find->right;
        }
    }

    // if(find->key == key) {return find;}
    
    return NULL;
}

// 3-2. 노드 탐색 (최솟값 노드)
node_t* rbtree_min(const rbtree* t){
    node_t* min = t->root;

    if(min == t->nil){
        return NULL;
    }

    while(min->left != t->nil){
        min = min->left;
    }

    return min;
}

// 3-3. 노드 탐색 (최댓값 노드)
node_t* rbtree_max(const rbtree* t){
    node_t* max = t->root;

    if(max == t->nil){
        return NULL;
    }

    while(max->right != t->nil){
        max = max->right;
    }

    return max;
}

// 4-1. 트리를 배열로 변환
int rbtree_to_array(const rbtree* t, key_t* arr, const size_t n){
    node_t* cur = t->root;
    int size = 0;
    int* sizePtr = &size;

    if(cur == t->nil){
        return -1;
    }

    rbtree_inorder(t, cur, arr, n, sizePtr);    

    return 0;
}

// 4-2. 트리를 배열로 변환 (2) - 중위 순회
void rbtree_inorder(const rbtree* t, node_t* n, key_t* arr, size_t size, int* sizePtr){

    // 종료 조건
    if(n == t->nil){
        return;
    }

    rbtree_inorder(t, n->left, arr, size, sizePtr);
    arr[*sizePtr] = n->key;
    *(sizePtr) += 1;
    rbtree_inorder(t, n->right, arr, size, sizePtr);
}

// 5-1. 노드 삭제
int rbtree_erase(rbtree* t, node_t* z){
    node_t* y = z;     // y는 삭제 노드 or 이동한 노드(의 위치)
    node_t* x = NULL;  // x는 계승 노드 (y의 원래 위치로 이동) -> 아니면 임시 보관?
    color_t y_origin_color = y->color;

    // 노드의 자식이 1개 또는 X 때
    if(z->left == t->nil){
        x = z->right;
        rbtree_transplant(t, z, x);

    }
    else if(z->right == t->nil){
        x = z->left;
        rbtree_transplant(t, z, x);
    }

    // 노드의 자식이 2개일 때
    else{
        // y = rbtree_min(z->right); // 오른쪽 서브트리에서 가장 작은 값을 y로 넣음
        y = z->right;
        while(y->left != t->nil){
            y = y->left;
        }

        y_origin_color = y->color;
        x = y->right;

        if(y->parent == z)
            x->parent = y;
        else{
            rbtree_transplant(t, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        rbtree_transplant(t, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    free(z);
    if(y_origin_color == RBTREE_BLACK) 
        rbtree_erase_fixup(t, x);
    return 0;
}

void rbtree_erase_fixup(rbtree* t, node_t* x){

    while (x != t->root && x->color == RBTREE_BLACK) {
        // CASE 1 ~ 4 : LEFT CASE
        if (x == x->parent->left){
        node_t *w = x->parent->right;
        
        // CASE 1 : x의 형제 w가 적색인 경우
        if (w->color == RBTREE_RED){
            w->color = RBTREE_BLACK;
            x->parent->color = RBTREE_RED;
            left_rotate(t, x->parent);
            w = x->parent->right;
        }

        // CASE 2 : x의 형제 w는 흑색이고 w의 두 지식이 모두 흑색인 경우
        if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
            w->color = RBTREE_RED;
            x = x->parent;
        }

        // CASE 3 : x의 형제 w는 흑색, w의 왼쪽 자식은 적색, w의 오른쪽 자신은 흑색인 경우
        else{ 
            if (w->right->color == RBTREE_BLACK) {
                w->left->color = RBTREE_BLACK;
                w->color = RBTREE_RED;
                right_rotate(t, w);
                w = x->parent->right;
            }

            // CASE 4 : x의 형제 w는 흑색이고 w의 오른쪽 자식은 적색인 경우
            w->color = x->parent->color;
            x->parent->color = RBTREE_BLACK;
            w->right->color = RBTREE_BLACK;
            left_rotate(t, x->parent);
            x = t->root;
        }
        }
        // CASE 5 ~ 8 : RIGHT CASE
        else {
        node_t *w = x->parent->left;

        // CASE 5 : x의 형제 w가 적색인 경우
        if (w->color == RBTREE_RED){
            w->color = RBTREE_BLACK;
            x->parent->color = RBTREE_RED;
            right_rotate(t, x->parent);
            w = x->parent->left;
        }

        // CASE 6 : x의 형제 w는 흑색이고 w의 두 지식이 모두 흑색인 경우
        if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
            w->color = RBTREE_RED;
            x = x->parent;
        }

        // CASE 7 : x의 형제 w는 흑색, w의 왼쪽 자식은 적색, w의 오른쪽 자신은 흑색인 경우
        else 
        {
            if (w->left->color == RBTREE_BLACK) {
                w->right->color = RBTREE_BLACK;
                w->color = RBTREE_RED;
                left_rotate(t, w);
                w = x->parent->left;
                }

            // CASE 8 : x의 형제 w는 흑색이고 w의 오른쪽 자식은 적색인 경우
            w->color = x->parent->color;
            x->parent->color = RBTREE_BLACK;
            w->left->color = RBTREE_BLACK;
            right_rotate(t, x->parent);
            x = t->root;
        }
        }
    }

  x->color = RBTREE_BLACK;
 
}

void rbtree_transplant(rbtree* t, node_t* u, node_t* v){
    if(u->parent == t->nil)
        t->root = v;
    else if(u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;

    v->parent = u->parent;
}






