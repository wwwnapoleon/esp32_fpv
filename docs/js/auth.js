// ============================================================
//  Аутентификация
//  - Юзер: localStorage (ФИО + цвет)
//  - Админ: Supabase Auth (email + пароль)
// ============================================================

// ---------- ЮЗЕР (ученик) ----------

// Сохранить юзера
function setUser(user) {
  if (user) {
    localStorage.setItem('user', JSON.stringify(user));
  } else {
    localStorage.removeItem('user');
  }
}

// Получить юзера из localStorage
function getUser() {
  try {
    const json = localStorage.getItem('user');
    return json ? JSON.parse(json) : null;
  } catch (e) {
    return null;
  }
}

// Выход юзера
function logoutUser() {
  localStorage.removeItem('user');
  window.location.href = 'index.html';
}

// ---------- АДМИН (Supabase Auth) ----------

// Проверка, залогинен ли админ
async function getAdminUser() {
  if (typeof SB === 'undefined' || !SB) return null;
  try {
    const { data: { user } } = await SB.auth.getUser();
    return user;
  } catch (e) {
    return null;
  }
}

// Выход админа
async function logoutAdmin() {
  if (SB) {
    await SB.auth.signOut();
  }
  sessionStorage.removeItem('admin');
  window.location.href = 'admin-login.html';
}

// ---------- ЗАЩИТА СТРАНИЦ ----------

// Защита страницы проекта (только для админа)
async function requireAdmin() {
  const user = await getAdminUser();
  if (!user) {
    window.location.href = 'admin-login.html';
    return null;
  }
  return user;
}

// ---------- ОБНОВЛЕНИЕ ТОПБАРА ----------

// Обновление топбара на ИНФО-сайте
function updateUserTopbar() {
  const loginBtn = document.querySelector('.btn-login');
  if (!loginBtn) return;

  const user = getUser();

  if (user) {
    // Показываем имя юзера
    loginBtn.outerHTML = `
      <div class="user-menu" id="user-menu">
        <div class="user-avatar" style="background: ${user.color || '#3fb950'};">
          ${(user.fio || '?').charAt(0).toUpperCase()}
        </div>
        <div class="user-name">${user.fio || 'Ученик'}</div>
        <button class="btn-logout-small" id="logout-btn" title="Выйти">→</button>
      </div>
    `;
    document.getElementById('logout-btn')?.addEventListener('click', logoutUser);
  }
  // Если не залогинен — оставляем кнопку «Войти»
}

// Обновление топбара на СТРАНИЦЕ ПРОЕКТА (для админа)
async function updateAdminTopbar() {
  const loginBtn = document.querySelector('.btn-login');
  if (!loginBtn) return;

  const user = await getAdminUser();
  if (!user) return;

  loginBtn.outerHTML = `
    <div class="user-menu" id="user-menu">
      <div class="user-avatar" style="background: #3fb950;">👑</div>
      <div class="user-name">${user.email.split('@')[0]}</div>
      <button class="btn-logout-small" id="logout-btn" title="Выйти">→</button>
    </div>
  `;
  document.getElementById('logout-btn')?.addEventListener('click', logoutAdmin);
}

// Автозапуск: обновляем топбар, если он есть
document.addEventListener('DOMContentLoaded', () => {
  // Определяем тип страницы
  const isProjectPage = document.body.classList.contains('project-page') ||
                        document.body.classList.contains('fpv-page') ||
                        document.body.classList.contains('database-page');

  if (isProjectPage) {
    updateAdminTopbar();
  } else {
    updateUserTopbar();
  }
});
