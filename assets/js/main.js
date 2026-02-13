// Theme toggle (persist in localStorage)
(function () {
    const root = document.documentElement;
    const userPref = localStorage.getItem('theme');
    if (userPref) root.setAttribute('data-theme', userPref);
    const btn = document.getElementById('themeToggle');
    if (btn) {
        btn.addEventListener('click', () => {
            const current = root.getAttribute('data-theme') === 'light' ? 'dark' : 'light';
            root.setAttribute('data-theme', current);
            localStorage.setItem('theme', current);
            btn.setAttribute('aria-pressed', String(current !== 'dark'));
        });
    }
})();


// Year in footer
(function () {
    const y = document.getElementById('year');
    if (y) y.textContent = new Date().getFullYear();
})();


// Mobile nav toggle
(function () {
    const toggle = document.getElementById('navToggle');
    const menu = document.getElementById('navMenu');
    if (!toggle || !menu) return;
    toggle.addEventListener('click', () => {
        const expanded = toggle.getAttribute('aria-expanded') === 'true';
        toggle.setAttribute('aria-expanded', String(!expanded));
        menu.style.display = expanded ? 'none' : 'flex';
    });
})();


// Simple filter by input and tags
(function () {
    const grid = document.getElementById('artifactGrid');
    const input = document.getElementById('filterInput');
    const tags = document.querySelectorAll('.tag[data-filter]');
    if (!grid) return;


    function applyFilter(q) {
        const query = (q || '').trim().toLowerCase();
        const cards = grid.querySelectorAll('.artifact');
        cards.forEach(card => {
            const hay = (card.textContent + ' ' + (card.getAttribute('data-tags') || ''))
                .toLowerCase();
            card.style.display = hay.includes(query) ? '' : 'none';
        });
    }


    if (input) input.addEventListener('input', (e) => applyFilter(e.target.value));
    tags.forEach(tag => tag.addEventListener('click', () => applyFilter(tag.dataset.filter)));
})();


// Modals (open/close)
(function () {
    function openModal(targetSel) {
        const modal = document.querySelector(targetSel);
        if (!modal) return;
        modal.hidden = false;
            modal.querySelector('[data-modal-close]')?.focus();
        }
        // Example usage: openModal('#myModal');
    }) ();
