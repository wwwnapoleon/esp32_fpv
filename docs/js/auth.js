// ============================================================
//  Аутентификация: QR (гость) + логин/пароль (админ)
// ============================================================

// ---- На странице index.html ----
const loginForm = document.getElementById('admin-login');

if (loginForm) {
  loginForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    const email = document.getElementById('email').value.trim();
    const password = document.getElementById('password').value;
    const errorEl = document.getElementById('login-error');
    errorEl.textContent = '';

    if (!supabase) {
      errorEl.textContent = 'Supabase не настроен. Заполни config.js';
      return;
    }

    try {
      const { data, error } = await supabase.auth.signInWithPassword({
        email, password
      });

      if (error) throw error;

      // Сохранить сессию и перейти на dashboard с ролью admin
      sessionStorage.setItem('user', JSON.stringify({
        role: 'admin',
        email: data.user.email,
        id: data.user.id
      }));
      window.location.href = 'dashboard.html';

    } catch (err) {
      errorEl.textContent = '❌ ' + (err.message || 'Ошибка входа');
    }
  });
}

// ---- Проверка роли на dashboard и других страницах ----
function checkAccess(requiredRole) {
  const urlParams = new URLSearchParams(window.location.search);
  const roleFromUrl = urlParams.get('role');

  // Гость по QR
  if (roleFromUrl === 'guest') {
    sessionStorage.setItem('user', JSON.stringify({ role: 'guest' }));
    return { role: 'guest' };
  }

  // Проверка сохранённой сессии
  const userJson = sessionStorage.getItem('user');
  if (!userJson) {
    window.location.href = 'index.html';
    return null;
  }

  const user = JSON.parse(userJson);

  // Если нужна роль admin, а у пользователя guest — редирект
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
  if (supabase) supabase.auth.signOut();
  window.location.href = 'index.html';
}
