// ============================================================
//  Работа с базой данных «ЭКО-ШКОЛА»
//  Пока нет Supabase — показываем демо-данные
// ============================================================

const user = checkAccess();
const isAdmin = user && user.role === 'admin';

// Показать админские элементы
if (isAdmin) {
  document.querySelectorAll('.btn-admin, .col-admin').forEach(el => {
    el.style.display = '';
  });
  document.getElementById('user-role').textContent = '👑 Админ';
} else {
  document.getElementById('user-role').textContent = '👤 Гость';
}

// Демо-данные (пока нет Supabase)
const DEMO_DATA = [
  { id: 1, name: 'Озеленение школьного двора',  members: '5 "А", 6 "Б"',     date: '2025-04-15', status: 'Активен' },
  { id: 2, name: 'Раздельный сбор мусора',      members: '7 "А"',            date: '2025-03-20', status: 'Завершён' },
  { id: 3, name: 'Экологический патруль',       members: '8 "В"',            date: '2025-05-01', status: 'Активен' },
  { id: 4, name: 'Сбор макулатуры',             members: '1–11 классы',      date: '2025-02-10', status: 'Завершён' },
  { id: 5, name: 'Гербарий редких растений',    members: '9 "А", 10 "Б"',    date: '2025-06-05', status: 'Активен' }
];

// Загрузка данных
async function loadData() {
  const statusEl = document.getElementById('db-status');
  const tbody = document.getElementById('data-body');

  // Если Supabase есть — грузим оттуда
  if (supabase) {
    statusEl.textContent = '⏳ Загрузка из Supabase...';
    try {
      const { data, error } = await supabase
        .from('eco_school')
        .select('*')
        .order('id');

      if (error) throw error;
      renderTable(data);
      statusEl.textContent = `✅ Загружено из Supabase: ${data.length} записей`;
    } catch (err) {
      statusEl.textContent = '❌ Ошибка: ' + err.message;
      renderTable(DEMO_DATA);
    }
  } else {
    // Демо-режим
    statusEl.textContent = 'ℹ️ Демо-режим (Supabase не настроен). ' +
                           'Показаны примерные данные.';
    renderTable(DEMO_DATA);
  }
}

// Рендер таблицы
function renderTable(rows) {
  const tbody = document.getElementById('data-body');
  if (!rows.length) {
    tbody.innerHTML = '<tr><td colspan="6" class="empty">Нет данных</td></tr>';
    return;
  }

  tbody.innerHTML = rows.map(r => `
    <tr>
      <td>${r.id}</td>
      <td><strong>${r.name}</strong></td>
      <td>${r.members}</td>
      <td>${r.date}</td>
      <td><span class="status-badge">${r.status}</span></td>
      ${isAdmin ? `<td class="col-admin">
        <button onclick="editRow(${r.id})" class="btn-sm">✏️</button>
        <button onclick="deleteRow(${r.id})" class="btn-sm btn-danger">🗑️</button>
      </td>` : ''}
    </tr>
  `).join('');
}

// Поиск
document.getElementById('search').addEventListener('input', (e) => {
  const q = e.target.value.toLowerCase();
  const rows = document.querySelectorAll('#data-body tr');
  rows.forEach(row => {
    row.style.display = row.textContent.toLowerCase().includes(q) ? '' : 'none';
  });
});

// Кнопки (заглушки для админов)
function editRow(id) {
  alert('Редактирование записи #' + id + ' — в разработке');
}
function deleteRow(id) {
  if (confirm('Удалить запись #' + id + '?')) {
    alert('Удаление #' + id + ' — в разработке');
  }
}

document.getElementById('refresh').addEventListener('click', loadData);
document.getElementById('add-row')?.addEventListener('click', () => {
  alert('Добавление записи — в разработке');
});

// Загрузить при старте
loadData();
