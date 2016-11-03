extern unsigned char line_buffer_buffer_id;
extern unsigned int line_buffer_line_number;
extern unsigned char line_buffer[255];
extern unsigned char line_buffer_length;
extern unsigned char line_buffer_original_length;
extern unsigned char line_buffer_dirty;


unsigned int line_find_offset(unsigned char buffer_id, unsigned int line_number);
unsigned char line_fetch(unsigned char buffer_id, unsigned int line_number);
