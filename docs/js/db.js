// ============================================================
//  Работа с базой данных «ЭКО-ШКОЛА»
//  Использует SB (см. config.js), НЕ supabase
// ============================================================

var user = checkAccess();
var isAdmin = user && user.role === 'admin';

if (isAdmin) {
  document.querySelectorAll('.btn-admin, .col-admin').forEach(function(el) {
    el.style.display = '';
  });
  document.getElementById('user-role').textContent = '👑 Админ';
} else {
  document.getElementById('user-role').textContent = '👤 Гость';
}

// Демо-данные
var DEMO_DATA = [
  { id: 1, name: 'Озеленение школьного двора',  members: '5 "А", 6 "Б"',  date: '2025-04-15', status: 'Активен' },
  { id: 2, name: 'Раздельный сбор мусора',      members: '7 "А"',         date: '2025-03-20', status: 'Завершён' },
  { id: 3, name: 'Экологический патруль',       members: '8 "В"',         date: '2025-05-01', status: 'Активен' },
  { id: 4, name: 'Сбор макулатуры',             members: '1–11 классы',   date: '2025-02-10', status: 'Завершён' },
  { id: 5, name: 'Гербарий редких растений',    members: '9 "А", 10 "Б"', date: '2025-06-05', status: 'Активен' }
];

async function loadData() {
  var statusEl = document.getElementById('db-status');

  // ---- Supabase режим ----
  if (SB) {
    statusEl.textContent = '⏳ Загрузка из Supabase...';
    try {
      var result = await SB.from('eco_school').select('*').order('id');
      if (result.error) throw result.error;

      renderTable(result.data || []);
      statusEl.textContent = '✅ Загружено из Supabase: ' +
                             result.data.length + ' записей';
    } catch (err) {
      console.error('Supabase error:', err);
      statusEl.textContent = '❌ Ошибка Supabase: ' + err.message +
                             ' — показаны демо-данные';
      renderTable(DEMO_DATA);
    }
    return;
  }

  // ---- Демо режим ----
  statusEl.textContent =
    'ℹ️ Демо-режим. Supabase не настроен — показаны примерные данные.';
  renderTable(DEMO_DATA);
}

function renderTable(rows) {
  var tbody = document.getElementById('data-body');

  if (!rows || !rows.length) {
    tbody.innerHTML = '<tr><td colspan="6" class="empty">Нет данных</td></tr>';
    return;
  }

  tbody.innerHTML = rows.map(function(r) {
    return '<tr>' +
      '<td>' + r.id + '</td>' +
      '<td><strong>' + r.name + '</strong></td>' +
      '<td>' + (r.members || '—') + '</td>' +
      '<td>' + (r.date || '—') + '</td>' +
      '<td><span class="status-badge">' + (r.status || '—') + '</span></td>' +
      (isAdmin
        ? '<td class="col-admin">' +
            '<button onclick="editRow(' + r.id + ')" class="btn-sm">✏️</button> ' +
            '<button onclick="deleteRow(' + r.id + ')" class="btn-sm btn-danger">🗑️</button>' +
          '</td>'
        : '') +
      '</tr>';
  }).join('');
}

document.getElementById('search').addEventListener('input', function(e) {
  var q = e.target.value.toLowerCase();
  document.querySelectorAll('#data-body tr').forEach(function(row) {
    row.style.display = row.textContent.toLowerCase().indexOf(q) !== -1
      ? '' : 'none';
  });
});

function editRow(id) {
  alert('Редактирование #' + id + ' — в разработке');
}
function deleteRow(id) {
  if (confirm('Удалить #' + id + '?')) {
    alert('Удаление #' + id + ' — в разработке');
  }
}

document.getElementById('refresh').addEventListener('click', loadData);
document.getElementById('add-row').addEventListener('click', function() {
  alert('Добавление — в разработке');
});

loadData();
