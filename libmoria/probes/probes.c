#if 0
void explore_edge_probe()
{
  // in explore_edge
  printf("[e] %p d %d t %d v %d 0x%08x %*s ", e, distance, travel, visit, e->attrs, distance, "");
  if(e->attrs & MORIA_EDGE_HYPER) {
    printf("{");
    int x;
    for(x = 0; x < e->Alen; x++) {
      printf(" %.*s", e->Alist[x].v->label_len, e->Alist[x].v->label);
    }
    printf("} -> {");
    for(x = 0; x < e->Blen; x++) {
      printf(" %.*s", e->Blist[x].v->label_len, e->Blist[x].v->label);
    }
    printf("}");
  } else {
    printf("%.*s -> %.*s\n", e->A->label_len, e->A->label, e->B->label_len, e->B->label);
  }
}

// in explore_vertex
void explore_vertex_probe()
{
  printf("[v] %p d %d t %d v %d 0x%08x %*s %.*s\n", v, distance, travel, visit, v->attrs, distance, "", v->label_len, v->label);
}
#endif
