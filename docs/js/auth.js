// ============================================================
//  Аутентификация
//  Использует SB (см. config.js), НЕ supabase
// ============================================================

var loginForm = document.getElementById('admin-login');

if (loginForm) {
  loginForm.addEventListener('submit', async function(e) {
    e.preventDefault();
    var email    = document.getElementById('email').value.trim();
    var password = document.getElementById('password').value;
    var errorEl  = document.getElementById('login-error');
    errorEl.textContent = '';

    if (!SB) {
      errorEl.textContent =
        '⚠️ Supabase не настроен. Вход только через QR (гость).';
      return;
    }

    try {
      var result = await SB.auth.signInWithPassword({
        email: email,
        password: password
      });

      if (result.error) throw result.error;

      sessionStorage.setItem('user', JSON.stringify({
        role: 'admin',
        email: result.data.user.email,
        id: result.data.user.id
      }));

      window.location.href = 'dashboard.html';

    } catch (err) {
      console.error('Login error:', err);
      errorEl.textContent = '❌ ' + (err.message || 'Ошибка входа');
    }
  });
}

function checkAccess(requiredRole) {
  var urlParams = new URLSearchParams(window.location.search);
  var roleFromUrl = urlParams.get('role');

  if (roleFromUrl === 'guest') {
    sessionStorage.setItem('user', JSON.stringify({ role: 'guest' }));
    return { role: 'guest' };
  }

  var userJson = sessionStorage.getItem('user');
  if (!userJson) {
    window.location.href = 'index.html';
    return null;
  }

  var user = JSON.parse(userJson);

  if (requiredRole === 'admin' && user.role !== 'admin') {
    alert('Доступ только для администраторов');
    window.location.href = 'dashboard.html?role=guest';
    return null;
  }

  return user;
}

function logout() {
  sessionStorage.removeItem('user');
  if (SB) SB.auth.signOut();
  window.location.href = 'index.html';
}
