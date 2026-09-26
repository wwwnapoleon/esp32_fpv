// ============================================================
//  Аутентификация: QR (гость) + логин/пароль (админ)
//  Использует SB (см. config.js)
// ============================================================

// ---- Форма логина на index.html ----
const loginForm = document.getElementById('admin-login');

if (loginForm) {
  loginForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    const email    = document.getElementById('email').value.trim();
    const password = document.getElementById('password').value;
    const errorEl  = document.getElementById('login-error');
    errorEl.textContent = '';

    if (!SB) {
      errorEl.textContent =
        '⚠️ Supabase не настроен. Вход для админов недоступен. ' +
        'Заполни SUPABASE_URL и SUPABASE_ANON_KEY в config.js';
      return;
    }

    try {
      const { data, error } = await SB.auth.signInWithPassword({
        email, password
      });

      if (error) throw error;

      sessionStorage.setItem('user', JSON.stringify({
        role: 'admin',
        email: data.user.email,
        id: data.user.id
      }));

      window.location.href = 'dashboard.html';

    } catch (err) {
      console.error('Login error:', err);
      errorEl.textContent = '❌ ' + (err.message || 'Ошибка входа');
    }
  });
}

// ---- Проверка роли ----
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
    window.location.href = 'index.html';
    return null;
  }

  const user = JSON.parse(userJson);

  if (requiredRole === 'admin' && user.role !== 'admin') {
    alert('Доступ только для администраторов');
    window.location.href = 'dashboard.html?role=guest';
    return null;
  }

  return user;
}

// ---- Выход ----
function logout() {
  sessionStorage.removeItem('user');
  if (SB) SB.auth.signOut();
  window.location.href = 'index.html';
}
