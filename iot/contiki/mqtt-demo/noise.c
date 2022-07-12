#define K 80.0
#define WINDOW 6

float readings_window[6] = {0};
int cur_reading = 0;
float cur_reading_sum = 0;

// Simulated reading. A real world application would process a sensor reading here
static float
get_noise_reading(void)
{
  float reading = (float)rand() / (float)RAND_MAX;
  reading *= 70.f;
  reading += 30.f;

  return reading;
}

static float
get_noise_average(void)
{
  float reading = get_noise_reading();
  cur_reading_sum -= readings_window[cur_reading];
  readings_window[cur_reading] = reading;
  cur_reading_sum += reading;
  cur_reading = (cur_reading + 1) % WINDOW;

  float average = cur_reading_sum / (float)WINDOW;

  return average > K ? reading : average;
}
/*---------------------------------------------------------------------------*/
static void
publish(void)
{
  /* Publish MQTT topic */
  int len;
  int remaining = APP_BUFFER_SIZE;

  seq_nr_value++;

  buf_ptr = app_buffer;

  len = snprintf(buf_ptr, remaining,
                 "{"
                 "\"lat\":%f,"
                 "\"lon\":%f,"
                 "\"noise\":%f,"
                 "\"reliability\":%f"
                 "}",
                 lat, lon, get_noise_average(), (float)rand() / (float)RAND_MAX);

  if (len < 0 || len >= remaining)
  {
    LOG_ERR("Buffer too short. Have %d, need %d + \\0\n", remaining, len);
    return;
  }

  remaining -= len;
  buf_ptr += len;

  mqtt_publish(&conn, NULL, pub_topic, (uint8_t *)app_buffer,
               strlen(app_buffer), MQTT_QOS_LEVEL_0, MQTT_RETAIN_OFF);

  LOG_INFO("Publish sent out!\n");
}