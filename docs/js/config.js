// ============================================================
//  Конфигурация проекта
//  Переменная называется sb (supabase), а не supabase —
//  чтобы не конфликтовать с библиотекой window.supabase
// ============================================================

const CONFIG = {
  // === Supabase ===
  // Оставь пустыми, если хочешь работать в ДЕМО-режиме
  SUPABASE_URL: 'https://database_eco_school.supabase.co',           // например: 'https://xxxx.supabase.co'
  SUPABASE_ANON_KEY: 'PvMMHvE3RQkNtTVcQCCzQ_rEpFx2HY',      // например: 'eyJhbGci...'

  // === ESP32 FPV камера ===
  FPV_STREAM_URL:   'http://192.168.4.1/stream',
  FPV_SNAPSHOT_URL: 'http://192.168.4.1/capture',
  FPV_STATUS_URL:   'http://192.168.4.1/status',

  // === Название БД ===
  DB_NAME: 'ЭКО-ШКОЛА'
};

// ============================================================
//  Инициализация Supabase
//  Переменная называется SB (глобальная), чтобы не путать
//  с window.supabase (это библиотека из CDN)
// ============================================================

let SB = null;

function isSupabaseConfigured() {
  return CONFIG.SUPABASE_URL &&
         CONFIG.SUPABASE_URL.startsWith('https://') &&
         CONFIG.SUPABASE_ANON_KEY &&
         CONFIG.SUPABASE_ANON_KEY.length > 50;
}

if (isSupabaseConfigured()) {
  if (typeof window.supabase === 'undefined') {
    console.error('❌ Библиотека Supabase не загружена. ' +
                  'Проверь <script src="...supabase-js...">');
  } else {
    try {
      SB = window.supabase.createClient(
        CONFIG.SUPABASE_URL,
        CONFIG.SUPABASE_ANON_KEY
      );
      console.log('✅ Supabase подключён:', CONFIG.SUPABASE_URL);
    } catch (e) {
      console.error('❌ Ошибка инициализации Supabase:', e);
      SB = null;
    }
  }
} else {
  console.info('ℹ️ Supabase не настроен — работаем в демо-режиме');
}
