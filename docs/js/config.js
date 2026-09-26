// ============================================================
//  Конфигурация проекта
//  ВАЖНО: переменную называем SB, а не supabase
//  (иначе конфликт с библиотекой window.supabase из CDN)
// ============================================================

const CONFIG = {
  // === Supabase (заполни, если хочешь реальную БД) ===
  SUPABASE_URL: 'https://database_eco_school.supabase.co',           // 'https://xxxx.supabase.co'
  SUPABASE_ANON_KEY: 'PvMMHvE3RQkNtTVcQCCzQ_rEpFx2HY',      // 'eyJhbGci...'

  // === ESP32 FPV ===
  FPV_STREAM_URL:   'http://192.168.4.1/stream',
  FPV_SNAPSHOT_URL: 'http://192.168.4.1/capture',
  FPV_STATUS_URL:   'http://192.168.4.1/status',

  DB_NAME: 'ЭКО-ШКОЛА'
};

// ============================================================
//  Глобальная переменная нашего клиента — SB
//  (НЕ supabase! чтобы не конфликтовать с CDN-библиотекой)
// ============================================================

var SB = null;   // ← используем var, а не let/const

// Проверка: настроены ли ключи
function isSupabaseConfigured() {
  return Boolean(
    CONFIG.SUPABASE_URL &&
    CONFIG.SUPABASE_URL.indexOf('https://') === 0 &&
    CONFIG.SUPABASE_ANON_KEY &&
    CONFIG.SUPABASE_ANON_KEY.length > 50
  );
}

// Инициализация клиента Supabase
if (isSupabaseConfigured()) {
  if (typeof window.supabase === 'undefined') {
    console.error('❌ Библиотека Supabase не загружена. ' +
                  'Проверь <script src="...supabase-js@2">');
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
