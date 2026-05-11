# TODO.md 

## Arquitetura + Design
- [X] Definir quais data structures utilizar
- [X] Detalhar a arquitetura do sistema (ARCHITECTURE.md)
- [X] Criar classDiagram com a relação entre entidades (Mermaid)
- [X] Criar mockups da interface gráfica (Figma)
- [ ] Criar sequence diagram (opcional)


## Structs + Headers (.h)
- [X] Definir todos os .h em conjunto. (.h antes dos .c)


## Módulos (Structures)

### Hashmap - Responsável:
- [X] Criar struct hashmap_t
- [ ] Implementar DJB2 hash function 

### Technician - Responsável:
- [X] Criar structs (technician_t, technician_node_t, technician_list_t)
- [ ] technician_list_insert()
- [ ] technician_list_remove()
- [ ] technician_list_find_by_id()
- [ ] Integrar username_index (hashmap)

### Equipment - Responsável:
- [X] Criar structs (equipment_t, equipment_node_t, equipment_list_t)
- [ ] equipment_list_insert()
- [ ] equipment_list_remove()
- [ ] equipment_find_by_id/ip/mac()
- [ ] equipment_sort_by_status/type/location() (Merge Sort)
- [ ] Integrar id_index, ip_index, mac_index (hashmap)

### Incident - Responsável:
- [X] Criar structs (incident_t, incident_node_t, incident_queue_t)
- [ ] incident_queue_enqueue()
- [ ] incident_queue_dequeue()
- [ ] incident_queue_peek_head()
- [ ] incident_find_by_equipment_sensor()
- [ ] incident_filter_by_priority()

### Config - Responsável:
- [X] Criar structs  (config_t, config_node_t, config_stack_t)
- [ ] config_stack_push()
- [ ] config_stack_pop()
- [ ] config_stack_peek()
- [ ] config_stack_peek_n()
- [ ] config_filter_by_equipment()

### Sensor - Responsável:
- [X] Criar structs (sensor_t, sensor_node_t, sensor_list_t)
- [ ] sensor_insert_head()
- [ ] sensor_find_by_code()
- [ ] sensor_filter_anomalous()


## Persistence + Connectivity

### Persistence - Responsável:
- [ ] Implementar save/load_technician()
- [ ] Implementar save/load_equipment()
- [ ] Implementar save/load_incident()
- [ ] Implementar save/load_config()
- [ ] Implementar save/load_sensor()

### Connectivity - Responsável:
- [ ] Implementar ping()
- [ ] Implementar traceroute()
- [ ] Implementar parse do ping()


## Interface Gráfica (GTK4)
- [ ] Login
- [ ] Dashboard
- [ ] Módulo Technician
- [ ] Módulo Equipment
- [ ] Módulo Incident
- [ ] Módulo Config
- [ ] Módulo Sensor
- [ ] Módulo Connectivity
- [ ] Importar/Exportar relatórios


## Testes 
- [ ] Testar manualmente cada módulo e a interface
- [ ] Testar memory leaks com Valgrind/GDB


## Relatório (LaTeX)
- [ ] Criar estrutura (índice, cápitulos/secções) - Responsável:
- [ ] Introdução e descrição do problema/projeto - Responsável:
- [ ] Documentar data structures e algoritmos utilizados - Responsável:
- [ ] Documentar decisões de arquitetura - Responsável:
- [ ] Conclusão - Responsável:
