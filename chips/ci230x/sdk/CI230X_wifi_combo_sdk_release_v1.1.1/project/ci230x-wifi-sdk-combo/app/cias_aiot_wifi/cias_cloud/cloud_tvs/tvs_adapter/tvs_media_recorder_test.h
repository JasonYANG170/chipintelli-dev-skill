#ifndef __TVS_MEDIA_RECORDER_TEST_H__
#define __TVS_MEDIA_RECORDER_TEST_H__

int tvs_media_recorder_test_open(int bitrate, int channel);

int tvs_media_recorder_test_record(char* data_buffer, int data_buffer_len);

void tvs_media_recorder_test_close();

#endif
