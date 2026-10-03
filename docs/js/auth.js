// ============================================================
//  Аутентификация через Supabase
//  - Гость: QR-код
//  - Админ: email + пароль
// ============================================================

// ---- Форма логина ----
const loginForm = document.getElementById('admin-login');

if (loginForm) {
  loginForm.addEventListener('submit', async function(e) {
    e.preventDefault();
    const email    = document.getElementById('email').value.trim();
    const password = document.getElementById('password').value;
    const errorEl  = document.getElementById('login-error');
    errorEl.textContent = '';

    if (!SB) {
      errorEl.textContent = '⚠️ Supabase не настроен. Вход недоступен.';
      return;
    }

    errorEl.textContent = '⏳ Вход...';

    try {
      const { data, error } = await SB.auth.signInWithPassword({
        email: email,
        password: password
      });

      if (error) throw error;

      // Сохраняем сессию в localStorage (Supabase сам это делает)
      console.log('✅ Вход выполнен:', data.user.email);

      // Переходим на главную
      window.location.href = 'index.html';

    } catch (err) {
      console.error('Login error:', err);
      errorEl.textContent = '❌ ' + (err.message || 'Неверный логин или пароль');
    }
  });
}

// ---- Проверка, залогинен ли пользователь ----
async function getCurrentUser() {
  if (!SB) return null;
  try {
    const { data: { user } } = await SB.auth.getUser();
    return user;
  } catch (e) {
    return null;
  }
}

// ---- Проверка доступа ----
function checkAccess(requiredRole) {
  const urlParams = new URLSearchParams(window.location.search);
  const roleFromUrl = urlParams.get('role');

  // QR → гость
  if (roleFromUrl === 'guest') {
    sessionStorage.setItem('user', JSON.stringify({ role: 'guest' }));
    return { role: 'guest' };
  }

  const userJson = sessionStorage.getItem('user');
  if (!userJson) {
    window.location.href = 'login.html';
    return null;
  }

  const user = JSON.parse(userJson);
  if (requiredRole === 'admin' && user.role !== 'admin') {
    alert('Доступ только для администраторов');
    window.location.href = 'index.html';
    return null;
  }
  return user;
}

// ---- Выход ----
async function logout() {
  if (SB) {
    await SB.auth.signOut();
  }
  sessionStorage.removeItem('user');
  window.location.href = 'index.html';
}

// ---- Обновление UI в топбаре ----
async function updateUserUI() {
  const loginBtn = document.querySelector('.btn-login');
  if (!loginBtn) return;

  const user = await getCurrentUser();

  if (user) {
    // Заменяем кнопку «Войти» на имя + кнопку выхода
    loginBtn.outerHTML = `
      <div class="user-menu" id="user-menu">
        <div class="user-avatar" id="user-avatar">👑</div>
        <div class="user-name" id="user-name">${user.email.split('@')[0]}</div>
        <button class="btn-logout-small" id="logout-btn" title="Выйти">→</button>
      </div>
    `;

    // Слушатель на выход
    document.getElementById('logout-btn')?.addEventListener('click', logout);

    // Сохраняем роль в sessionStorage
    sessionStorage.setItem('user', JSON.stringify({
      role: 'admin',
      email: user.email,
      id: user.id
    }));
  }
}

// Запускаем обновление UI при загрузке
document.addEventListener('DOMContentLoaded', updateUserUI);
