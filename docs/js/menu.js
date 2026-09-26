// ============================================================
//  Боковое меню: открытие/закрытие, подпункты
// ============================================================

const sidebar  = document.getElementById('sidebar');
const overlay  = document.getElementById('sidebar-overlay');
const burger   = document.getElementById('burger');

function openMenu() {
  sidebar?.classList.add('open');
  overlay?.classList.add('visible');
  document.body.classList.add('menu-open');
}

function closeMenu() {
  sidebar?.classList.remove('open');
  overlay?.classList.remove('visible');
  document.body.classList.remove('menu-open');
}

burger?.addEventListener('click', () => {
  if (sidebar?.classList.contains('open')) closeMenu();
  else openMenu();
});

overlay?.addEventListener('click', closeMenu);

document.querySelectorAll('[data-toggle]').forEach(btn => {
  btn.addEventListener('click', () => {
    const target = btn.getAttribute('data-toggle');
    const submenu = document.getElementById('submenu-' + target);
    const arrow = btn.querySelector('.arrow');

    if (submenu) {
      const isOpen = submenu.classList.toggle('open');
      if (arrow) arrow.textContent = isOpen ? '▾' : '▸';
    }
  });
});

document.querySelectorAll('.submenu-item').forEach(link => {
  link.addEventListener('click', () => {
    if (window.innerWidth < 1024) closeMenu();
  });
});

document.addEventListener('keydown', (e) => {
  if (e.key === 'Escape') closeMenu();
});
