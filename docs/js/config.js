// ============================================================
//  Конфигурация проекта
//  Заполни своими ключами Supabase (получишь на supabase.com)
// ============================================================

const CONFIG = {
  // Supabase
  SUPABASE_URL: 'https://database_eco_school.supabase.co',
  SUPABASE_ANON_KEY: 'PvMMHvE3RQkNtTVcQCCzQ_rEpFx2HY',

  // ESP32 FPV камера
  // Если камера в той же сети, что и браузер — используй её IP
  // Если смотришь с телефона в AP-режиме — http://192.168.4.1
  FPV_STREAM_URL: 'http://192.168.4.1/stream',
  FPV_SNAPSHOT_URL: 'http://192.168.4.1/capture',
  FPV_STATUS_URL: 'http://192.168.4.1/status',

  // Название БД
  DB_NAME: 'ЭКО-ШКОЛА'
};

// Инициализация Supabase (если ключи заполнены)
let supabase = null;
if (CONFIG.SUPABASE_URL.includes('supabase.co') &&
    !CONFIG.SUPABASE_URL.includes('ВАШ-ПРОЕКТ')) {
  supabase = window.supabase.createClient(
    CONFIG.SUPABASE_URL,
    CONFIG.SUPABASE_ANON_KEY
  );
}
